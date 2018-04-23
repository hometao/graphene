//=============================================================================
// Copyright (C) 2013 by Graphics & Geometry Group, Bielefeld University
//=============================================================================


//== INCLUDES =================================================================


#include <iostream>
#include <sstream>

#include <graphene/gl/gl_includes.h>
#include <graphene/scene_graph/Object_node.h>
#include <graphene/geometry/Bounding_box.h>

#include <QInputDialog>
#include <QSettings>
#include <QMimeData>
#include <QAction>
#include <QLibrary>
#include <QPluginLoader>
#include <QFileDialog>
#include <QMessageBox>
#include <QToolBox>
#include <QToolBar>
#include <QColor>
#include <QColorDialog>
#include <QMenu>
#include <QFileInfo>

#include <graphene/macros.h>
#include <graphene/qt/Main_window.h>
#include <graphene/qt/events.h>
#include <graphene/qt/plugins/interfaces/Plugin_interface.h>
#include <graphene/qt/plugins/interfaces/IO_plugin_interface.h>
#include <graphene/qt/plugins/interfaces/Mouse_plugin_interface.h>
#include <graphene/qt/plugins/interfaces/Keyboard_plugin_interface.h>
#include <graphene/qt/plugins/interfaces/Manipulation_plugin_interface.h>
#include <graphene/surface_mesh/scene_graph/Surface_mesh_node.h>

#include "ui_About_graphene_dialog.h"


//== NAMESPACES ===============================================================

using ::QMenu;

namespace graphene {
	namespace qt {


		//== IMPLEMENTATION ===========================================================


		Main_window::
			Main_window() : QMainWindow()
		{
			setupUi(this);

			setWindowIcon(QIcon(":/graphene/icons/Graphene.xpm"));
			setAcceptDrops(true);

			qglviewer_ = new QGL_viewer_widget();
			setCentralWidget(qglviewer_);

			qglviewer_->setFocus();
			actionMoveMode->setChecked(true);

			scene_graph_ = new Scene_graph();
			qglviewer_->set_scene(scene_graph_);

			toolbox_ = toolBox;
			console_ = consoleTextEdit;
			toolbar_ = viewToolBar;
			qout_ = new Qdebug_stream(std::cout, console_, Qt::black);
			qerr_ = new Qdebug_stream(std::cerr, console_, Qt::red);

			while (toolbox_->count())
				toolbox_->removeItem(0);

			QAction* consoleViewAction = consoleDockWidget->toggleViewAction();
			consoleViewAction->setShortcut(QKeySequence("Ctrl+Shift+C"));
			menuWindow->addAction(consoleViewAction);

			QAction* toolbox_ViewAction = toolboxDockWidget->toggleViewAction();
			toolbox_ViewAction->setShortcut(QKeySequence("Ctrl+Shift+T"));
			menuWindow->addAction(toolbox_ViewAction);

			create_recent_files_menu();

			load_plugins();

			update_toolbox_shortcuts();

			statusBar()->showMessage(tr("Ready."));

			fps_label_ = new QLabel();
			qglviewer_->set_fps_label(fps_label_);
			statusBar()->addPermanentWidget(fps_label_);

			connect(this, SIGNAL(signal_scene_graph_changed()),
				this, SLOT(slot_update_statusbar()));

			QSettings settings;
			settings.setValue("currentFilename", 0);
		}


		//-----------------------------------------------------------------------------


		Main_window::
			~Main_window()
		{
			delete qglviewer_;
			delete scene_graph_;
			delete qout_;
			delete qerr_;
			delete fps_label_;
		}


		//-----------------------------------------------------------------------------


		void
			Main_window::
			create_recent_files_menu()
		{
			for (int i = 0; i < s_max_recent_files; ++i)
			{
				recent_file_actions[i] = new QAction(this);
				recent_file_actions[i]->setVisible(false);
				connect(recent_file_actions[i], SIGNAL(triggered()),
					this, SLOT(actionOpenRecentFile_triggered()));
			}

			separator = menuFile->insertSeparator(this->actionClose);
			QMenu *recent_files_menu = new QMenu("Open Recent File");

			for (int i = 0; i < s_max_recent_files; ++i)
				recent_files_menu->addAction(recent_file_actions[i]);

			update_recent_file_actions();

			menuFile->insertMenu(separator, recent_files_menu);
		}


		//-----------------------------------------------------------------------------


		void
			Main_window::
			open(const QString& filename,int model)
		{
			QFileInfo fileinfo(filename);
			Base_node* node = 0;
			QStringList types;

			if (!fileinfo.isReadable())
			{
				QMessageBox::critical(this,
					tr("Could not open file"),
					tr("%1 is not readable.").arg(filename));
			}

			foreach(IO_plugin_interface* plugin, io_plugins)
			{
				if (plugin->can_load(fileinfo))
				{
					types << plugin->data_types();
				}
			}


			// no plugin can read
			if (types.isEmpty()) return;

			QString selection;

			// exactly one can read
			if (types.size() == 1)
			{
				selection = types[0];
			}

			// several plugins can read -> choose file type
			else
			{
				bool ok;
				selection = QInputDialog::getItem(this,
					tr("QInputDialog::getItem()"),
					tr("Select data type"),
					types, 0, false, &ok);
				if (!ok) return;
			}

			foreach(IO_plugin_interface* plugin, io_plugins)
			{
				types = plugin->data_types();

				if (types.contains(selection) && plugin->can_load(fileinfo))
				{
					node = plugin->load(fileinfo,model);

					if (node)
					{
						break;
					}
				}
			}

			if (node)
			{
				emit(signal_scene_graph_changed());

				set_current_file(fileinfo.absoluteFilePath());

				foreach(Manipulation_plugin_interface* manipulation_plugin,
					manipulation_plugins_)
				{
					manipulation_plugin->update_manipulation_modes((Base_node*)scene_graph_->selected_node());
				}

				qglviewer_->new_node_loaded_triggered();
			}
			else
			{
				QMessageBox::critical(this,
					tr("Could not open file"),
					tr("%1 has an unknown file format.").arg(filename));
			}
		}

		void Main_window::on_actionLoadFeature_triggered()
		{
			QStringList filenames;

			QSettings settings;
			QString dir(settings.value("recentDir").toString());

			QFileDialog dialog(this, "Open feature line data", dir);
			dialog.setNameFilters(filters_);
			dialog.setFileMode(QFileDialog::ExistingFiles);

			if (dialog.exec())
			{
				filenames = dialog.selectedFiles();

				if (filenames.isEmpty())
				{
					return;
				}
				else
				{
					foreach(QString filename, filenames)
					{

						open(filename,1);
						settings.setValue("currentFilename", filename);
					}

					QFileInfo fileinfo(filenames.first());
					QSettings settings;
					QDir dir = fileinfo.absoluteDir();
					settings.setValue("recentDir", dir.absolutePath());
				}
			}

		}
		//-----------------------------------------------------------------------------


		void
			Main_window::
			set_current_file(const QString &filename)
		{
			current_file_name = filename;
			setWindowTitle("Graphene - " + QFileInfo(current_file_name).fileName());

			QSettings settings;
			QStringList files = settings.value("recentFileList").toStringList();
			files.removeAll(filename);
			files.prepend(filename);

			while (files.size() > s_max_recent_files)
			{
				files.removeLast();
			}

			settings.setValue("recentFileList", files);

			update_recent_file_actions();
		}


		//-----------------------------------------------------------------------------


		void
			Main_window::
			update_recent_file_actions()
		{
			QSettings settings;
			QStringList files = settings.value("recentFileList").toStringList();

			int numRecentFiles = std::min(files.size(),
				static_cast<int>(s_max_recent_files));

			for (int i = 0; i < numRecentFiles; ++i)
			{
				QString text = tr("&%1").arg(QFileInfo(files[i]).fileName());
				recent_file_actions[i]->setText(text);
				recent_file_actions[i]->setData(files[i]);
				recent_file_actions[i]->setVisible(true);
				recent_file_actions[i]->setShortcut("Ctrl+" + QString::number(i + 1));
			}

			for (int j = numRecentFiles; j < s_max_recent_files; ++j)
			{
				recent_file_actions[j]->setVisible(false);
			}

			separator->setVisible(numRecentFiles > 0);
		}


		//-----------------------------------------------------------------------------


		void
			Main_window::
			update_toolbox_shortcuts()
		{
			foreach(QAction* action, toolbox_actions_)
			{
				delete action;
			}

			toolbox_actions_.clear();

			for (int i(0); i < toolbox_->count(); i++)
			{
				QAction* action = new QAction(this);
				action->setShortcut("Alt+" + QString::number(i + 1));
				action->setData(i);
				action->setVisible(true);
				toolbox_actions_.push_back(action);
				this->addAction(action);
				connect(toolbox_actions_.back(), SIGNAL(triggered()),
					this, SLOT(actionSelectToolboxItem_triggered()));
			}
		}


		//-----------------------------------------------------------------------------


		void
			Main_window::
			actionSelectToolboxItem_triggered()
		{
			QAction *action = qobject_cast<QAction*>(sender());

			if (action)
			{
				toolbox_->setCurrentWidget(toolbox_->widget(action->data().toInt()));
			}
		}


		//-----------------------------------------------------------------------------


		void
			Main_window::
			on_actionOpen_triggered()
		{
			QStringList filenames;

			QSettings settings;
			QString dir(settings.value("recentDir").toString());

			QFileDialog dialog(this, "Open File", dir);
			dialog.setNameFilters(filters_);
			dialog.setFileMode(QFileDialog::ExistingFiles);

			if (dialog.exec())
			{
				filenames = dialog.selectedFiles();

				if (filenames.isEmpty())
				{
					return;
				}
				else
				{
					foreach(QString filename, filenames)
					{
						open(filename);
						settings.setValue("currentFilename", filename);
					}

					QFileInfo fileinfo(filenames.first());
					QSettings settings;
					QDir dir = fileinfo.absoluteDir();
					settings.setValue("recentDir", dir.absolutePath());
				}
			}
		}


		//-----------------------------------------------------------------------------


		void
			Main_window::
			actionOpenRecentFile_triggered()
		{
			QAction *action = qobject_cast<QAction *>(sender());

			if (action)
			{
				open(action->data().toString());
			}
		}


		//-----------------------------------------------------------------------------


		void
			Main_window::
			on_actionClose_triggered()
		{
			Base_node* node = (Base_node*)scene_graph_->selected_node();

			if (node)
			{
				delete node;
				emit(signal_scene_graph_changed());
				qglviewer_->updateGL();
				setWindowTitle("Graphene");

				QSettings settings;
				settings.setValue("currentFilename", 0);
			}
		}


		//-----------------------------------------------------------------------------


		void
			Main_window::
			on_actionSave_triggered()
		{
			Base_node* node = (Base_node*)scene_graph_->selected_node();

			if (node)
			{
				QFileInfo fileinfo(node->fileinfo().c_str());

				if (!fileinfo.exists())
				{
					on_actionSaveAs_triggered();
					return;
				}

				bool node_saved = false;

				foreach(IO_plugin_interface* plugin, io_plugins)
				{
					if (plugin->can_save(node, fileinfo))
					{
						node_saved = plugin->save(node, fileinfo);
					}
				}

				if (!node_saved)
				{
					QMessageBox::critical(this,
						tr("Could not save file"),
						tr("No plugin for writing file $1.")
						.arg("XXX"));
				}
			}
		}


		//-----------------------------------------------------------------------------


		void
			Main_window::
			on_actionSaveAs_triggered()
		{
			QStringList filenames;

			QSettings settings;
			QString dir(settings.value("recentDir").toString());

			QFileDialog dialog(this, "Save As", dir);
			dialog.setNameFilters(filters_);
			dialog.setFileMode(QFileDialog::AnyFile);
			dialog.setAcceptMode(QFileDialog::AcceptSave);

			if (dialog.exec())
				filenames = dialog.selectedFiles();

			if (filenames.isEmpty())
				return;

			QFileInfo fileinfo(filenames.first());
			Base_node * node = (Base_node*)scene_graph_->selected_node();
			bool node_saved = false;

			foreach(IO_plugin_interface* plugin, io_plugins)
			{
				if (plugin->can_save(node, fileinfo))
				{
					node_saved = plugin->save(node, fileinfo);
					set_current_file(fileinfo.absoluteFilePath());
					node->set_fileinfo(fileinfo.filePath().toStdString().data());
				}
			}

			if (!node_saved)
			{
				QMessageBox::critical(this,
					tr("Could not save file"),
					tr("File %1 has an unknown file format.")
					.arg(filenames.first()));
			}
		}


		//-----------------------------------------------------------------------------


		void
			Main_window::
			on_actionRevert_triggered()
		{
			Base_node* node = (Base_node*)scene_graph_->selected_node();

			if (node)
			{
				std::string filename = node->fileinfo();
				open(filename.c_str());
				delete node;
			}
		}


		//-----------------------------------------------------------------------------


		void
			Main_window::
			on_actionQuit_triggered()
		{
			emit signal_quit();
		}


		//-----------------------------------------------------------------------------


		void
			Main_window::
			on_actionFullscreen_triggered()
		{
			if (isFullScreen())
			{
				showNormal();
			}
			else
			{
				showFullScreen();
			}
		}


		//-----------------------------------------------------------------------------


		void
			Main_window::
			on_actionResetView_triggered()
		{
			qglviewer_->new_node_loaded_triggered();
		}


		//-----------------------------------------------------------------------------


		void
			Main_window::
			on_actionRestoreView_triggered()
		{
			qglviewer_->restore_view();
		}


		//-----------------------------------------------------------------------------


		void
			Main_window::
			on_actionSetView_triggered()
		{
			qglviewer_->set_view();
		}


		//-----------------------------------------------------------------------------


		void
			Main_window::
			on_actionSaveView_triggered()
		{
			QStringList filenames;

			QSettings settings;
			QString dir(settings.value("recentDir").toString());

			QFileDialog dialog(this, "Save As", dir);
			dialog.setNameFilters(QStringList("View files (*.view)"));
			dialog.setFileMode(QFileDialog::AnyFile);
			dialog.setAcceptMode(QFileDialog::AcceptSave);

			if (dialog.exec())
				filenames = dialog.selectedFiles();

			if (filenames.isEmpty())
				return;

			qglviewer_->save_view(filenames.first());
		}


		//-----------------------------------------------------------------------------


		void
			Main_window::
			on_actionLoadView_triggered()
		{
			QStringList filenames;

			QSettings settings;
			QString dir(settings.value("recentDir").toString());

			QFileDialog dialog(this, "Open File", dir);
			dialog.setNameFilters(QStringList("View files (*.view)"));
			dialog.setFileMode(QFileDialog::ExistingFiles);

			if (dialog.exec())
			{
				filenames = dialog.selectedFiles();

				if (filenames.isEmpty())
				{
					return;
				}
				else
				{
					qglviewer_->load_view(filenames.first());
					QSettings settings;
					QFileInfo fileinfo(filenames.first());
					QDir dir = fileinfo.absoluteDir();
					settings.setValue("recentDir", dir.absolutePath());
				}
			}
		}


		//-----------------------------------------------------------------------------


		void
			Main_window::
			on_actionScreenshot_triggered()
		{
			QStringList filenames;
			QSettings   settings;
			QString     dir(settings.value("recentDir").toString());
			QFileDialog dialog(this, "Save As", dir);

			dialog.setNameFilter(tr("Images (*.png *.xpm *.jpg)"));
			dialog.setFileMode(QFileDialog::AnyFile);
			dialog.setAcceptMode(QFileDialog::AcceptSave);

			if (dialog.exec())
				filenames = dialog.selectedFiles();

			if (filenames.isEmpty())
				return;

			qglviewer_->screen_shot(filenames.first());
		}


		//-----------------------------------------------------------------------------


		void
			Main_window::
			on_actionAbout_triggered()
		{
			QDialog *dialog = new QDialog();
			Ui::About_graphene_dialog *d = new Ui::About_graphene_dialog();
			d->setupUi(dialog);
			QSettings settings;
			d->versionLabel->setText("Version " +
				settings.value("ApplicationVersion").toString());
			dialog->exec();
		}


		//-----------------------------------------------------------------------------


		void
			Main_window::
			on_actionMoveMode_triggered()
		{
			qglviewer_->set_mouse_mode(move_mode);
			this->actionSelectionMode->setChecked(false);
			this->actionManipulationMode->setChecked(false);
		}


		//-----------------------------------------------------------------------------


		void
			Main_window::
			on_actionSelectionMode_triggered()
		{
			qglviewer_->set_mouse_mode(selection_mode);
			this->actionMoveMode->setChecked(false);
			this->actionManipulationMode->setChecked(false);
		}


		//-----------------------------------------------------------------------------


		void
			Main_window::
			on_actionManipulationMode_triggered()
		{
			qglviewer_->set_mouse_mode(manipulation_mode);
			this->actionMoveMode->setChecked(false);
			this->actionSelectionMode->setChecked(false);
		}

		void 
			Main_window::
			on_actionDilateLine_triggered()
		{
			int i = 0;
			i++;
			return;
		}
		//-----------------------------------------------------------------------------


		void
			Main_window::
			load_plugins()
		{
			QDir plugins_dir(qApp->applicationDirPath());

#if defined(Q_OS_MAC)
			if (plugins_dir.dirName() == "MacOS")
				plugins_dir.cd("../Plugins");

			QStringList filters;
			filters << "*.so";
			plugins_dir.setNameFilters(filters);
#endif

#if defined(Q_OS_WIN)
			QStringList filters;
			filters << "*.dll";
			plugins_dir.setNameFilters(filters);
#endif

#if defined(Q_OS_LINUX)
			QStringList filters;
			filters << "*.so";
			plugins_dir.setNameFilters(filters);
#endif

			foreach(QString filename, plugins_dir.entryList(QDir::Files))
			{
				QPluginLoader plugin_loader(plugins_dir.absoluteFilePath(filename));
				QObject* plugin = plugin_loader.instance();

				receivers.push_back(plugin);

				if (plugin)
				{
					Plugin_interface* pi = dynamic_cast<Plugin_interface*>(plugin);
					if (pi)
					{
						plugins.push_back(pi);
						pi->init(this);
						//LOG(Log_info) << pi->name().toStdString().data() << " loaded.\n";
					}

					IO_plugin_interface* iopi = qobject_cast<IO_plugin_interface*>(plugin);
					if (iopi)
					{
						io_plugins.push_back(iopi);
						filters_ << iopi->filters();
						filters_.removeDuplicates();
					}

					Mouse_plugin_interface *mpi = qobject_cast<Mouse_plugin_interface*>(plugin);
					if (mpi)
					{
						connect(qglviewer_, SIGNAL(signalMouseEvent(QMouseEvent*)),
							plugin, SLOT(slot_mouse_event(QMouseEvent*)));
					}

					Keyboard_plugin_interface *kpi = qobject_cast<Keyboard_plugin_interface*>(plugin);
					if (kpi)
					{
						connect(this, SIGNAL(signal_key_press_event(QKeyEvent*)),
							plugin, SLOT(slot_key_press_event(QKeyEvent*)));
					}

					Manipulation_plugin_interface *manipulation_plugin = qobject_cast<Manipulation_plugin_interface*>(plugin);
					if (manipulation_plugin)
					{
						manipulation_plugins_.push_back(manipulation_plugin);
					}
				}
			}

			// gather file name filter for all files
			QString all_filter = "All files (";

			for (int i(0); i < filters_.size(); i++)
			{
				QString filter = filters_[i];
				filter = filter.section(QRegExp("(\\s)"), 2, 2);
				filter = filter.replace("(", "");
				filter = filter.replace(")", "");
				all_filter += filter;

				if (i != filters_.size() - 1)
				{
					all_filter += " ";
				}
			}

			all_filter += ")";
			filters_.insert(0, all_filter);
		}


		//-----------------------------------------------------------------------------


		void
			Main_window::
			keyPressEvent(QKeyEvent* _e)
		{
			switch (_e->key())
			{
			case Qt::Key_Escape:
			{
				qglviewer_->toggle_mouse_mode();

				actionMoveMode->setChecked(false);
				actionSelectionMode->setChecked(false);
				actionManipulationMode->setChecked(false);

				switch (qglviewer_->current_mouse_mode_)
				{
				case move_mode:
					actionMoveMode->setChecked(true);
					break;
				case selection_mode:
					actionSelectionMode->setChecked(true);
					break;
				case manipulation_mode:
					actionManipulationMode->setChecked(true);
					break;
				default:
					break;
				}
				break;
			}
			case Qt::Key_M:
			{
				actionMoveMode->setChecked(true);
				actionSelectionMode->setChecked(false);
				actionManipulationMode->setChecked(false);
				qglviewer_->set_mouse_mode(move_mode);

				break;
			}
			case Qt::Key_S:
			{
				actionMoveMode->setChecked(false);
				actionSelectionMode->setChecked(true);
				actionManipulationMode->setChecked(false);
				qglviewer_->set_mouse_mode(selection_mode);

				break;
			}
			case Qt::Key_D:
			{
				actionMoveMode->setChecked(false);
				actionSelectionMode->setChecked(false);
				actionManipulationMode->setChecked(true);
				qglviewer_->set_mouse_mode(manipulation_mode);

				break;
			}

			}

			emit signal_key_press_event(_e);
		}


		//-----------------------------------------------------------------------------


		void
			Main_window::
			dragEnterEvent(QDragEnterEvent* _event)
		{
			if (_event->mimeData()->hasFormat("text/uri-list"))
				_event->acceptProposedAction();
		}


		//-----------------------------------------------------------------------------


		void
			Main_window::
			dropEvent(QDropEvent* _event)
		{
			QList<QUrl> urlList = _event->mimeData()->urls();

			for (int i = 0; i < urlList.size(); ++i)
				open(urlList.at(i).path());

			_event->acceptProposedAction();
		}


		//-----------------------------------------------------------------------------


		void
			Main_window::
			customEvent(QEvent *event)
		{
			if (event->type() == GEOMETRY_CHANGED)
			{
				foreach(QObject* receiver, receivers)
				{
					if (receiver)
					{
						QApplication::sendEvent(receiver, event);
					}
				}

				qglviewer_->model_changed_triggered();
				qglviewer_->updateGL();

				slot_update_statusbar();
			}
		}


		//-----------------------------------------------------------------------------


		void
			Main_window::
			slot_update_statusbar()
		{
			std::ostringstream s;

			const unsigned int n_objects = scene_graph_->objects().size();
			s << "Scene: " << n_objects << (n_objects == 1 ? " object" : " objects");

			if (n_objects)
			{
				geometry::Bounding_box bb = scene_graph_->bbox();
				s << ",  Size: "
					<< bb.max()[0] - bb.min()[0] << " x "
					<< bb.max()[1] - bb.min()[1] << " x "
					<< bb.max()[2] - bb.min()[2];

				Object_node *target = scene_graph_->selected_node();
				if (target)
				{
					QFileInfo file(target->fileinfo().c_str());
					s << ",  Target: " << file.fileName().toStdString();
					s << " (" << target->name() << ", " << target->info() << ")";
				}
			}

			statusBar()->showMessage(s.str().c_str());
		}

	

		//=============================================================================
	} // namespace qt
} // namespace graphene
  //=============================================================================
