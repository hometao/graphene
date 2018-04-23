#include <graphene/qt/plugins/povray_plugin/Scene_graph_converter.h>


// ---------------------------------------------------------------------------


namespace graphene {


// ---------------------------------------------------------------------------


const std::string convert_node_to_povray_format(Base_node*,std::map<Base_node*,std::string>&);

const std::string create_parent_directive(Base_node*, std::string);


// ---------------------------------------------------------------------------


Scene_graph_converter::
Scene_graph_converter(const Scene_graph* _graph,
                      const Povray_options _options
                      ) :
    graph_(_graph),
    options_(_options),
    pov_file_((options_.name_prefix + ".pov").c_str()),
    ini_file_((options_.name_prefix + ".ini").c_str())
{
}


// ---------------------------------------------------------------------------


bool
Scene_graph_converter::
start()
{
    create_pov_file();
    create_ini_file();

    return true;
}


// ---------------------------------------------------------------------------


void Scene_graph_converter::create_pov_file()
{
    // pov header
    //pov_file_ << "#version 3.7;" << std::endl;
    pov_file_ << "#version 3.6;" << std::endl;
    pov_file_ << "global_settings { assumed_gamma 1.0 ambient_light rgb <1.0,1.0,1.0> }" << std::endl;
    pov_file_ << "background { color rgb <1.0, 1.0, 1.0> }" << std::endl;

    // write node declarations and scene hierachy to pov-file
    pov_file_ << traverse(graph_->root_);

    pov_file_.close();
}


// ---------------------------------------------------------------------------


void Scene_graph_converter::create_ini_file()
{
    // write Povray file name
    ini_file_ << "Input_File_Name = "  << options_.name_prefix << ".pov" << std::endl;

    // Output dimension for now
    ini_file_ << "Width  = " << options_.width << std::endl;
    ini_file_ << "Height = " << options_.height << std::endl;

    // write antialias options
    ini_file_ << "Antialias = " << (options_.antialias ? "on" : "off") << std::endl;
    ini_file_ << "Sampling_Method = " << options_.sampling_method << std::endl;
    ini_file_ << "Antialias_Threshold = " << options_.threshold << std::endl;
    ini_file_ << "Antialias_Depth = " << options_.depth << std::endl;

    // write quality option
    ini_file_ << "Quality = " << options_.quality << std::endl;

    // write flags
    ini_file_ << "Verbose = " << (options_.verbose ? "on" : "off") << std::endl;
    ini_file_ << "Display = " << (options_.display ? "on" : "off") <<  std::endl;

    ini_file_.close();
}


// ---------------------------------------------------------------------------


void
Scene_graph_converter::
add_declaration_to_pov_file(Base_node* _node)
{

    // if the same node is used in more than one position in
    // the scene graph, make sure, only one declaration is
    // made for this node.

    // Node declaration already added?
    if (std::find(nodes_.begin(),nodes_.end(),_node) == nodes_.end())
    {
        // No, remember new node.
        nodes_.push_back(_node);

        // Add declaration to pov file.
        // Fill in the usage of _node into the map
        pov_file_ << convert_node_to_povray_format(_node, keyword_map_) << '\n';
    }

    // declaration already added, nothing to do.

}


// ---------------------------------------------------------------------------


std::string
Scene_graph_converter::
unions_of_children(Base_node* _node)
{

    // converts all children of _node into unions and adds a declaration for
    // every node

    std::string e;

    if  (_node->children_.size() > 0)
    {
        Base_node::Child_iterator it = _node->children_.begin();
        Base_node::Child_iterator it_end = _node->children_.end();

        for (; it != it_end; ++it)
        {
            e = e + traverse(*it);
        }
    }

    return e;

}


// ---------------------------------------------------------------------------


std::string
Scene_graph_converter::
traverse(Base_node* _node)
{

    // The scene graph tree is represented with unions.
    // Simplified construction:
    // * Around every node an union is created.
    // * Every child node is contained in its parents union.
    // * The content of a child node union is created like it's parent,
    //   but now the child node is the parent.
    // * The directive to use the parent node has to be the last entry
    //   in the union, because a parent node can be a modifier (texture,
    //   transform, ...) for its children. Modifiers are only allowed at
    //   the end of an union.
    //
    // Example for the representation of a parent node and its children
    // in the scene graph:
    //
    // union {
    //  union { child0  }
    //  union {   ...   }
    //  union { child n }
    //  use parent directive
    //  }
    //
    //  Every child node is also represented like the parent node.
    //
    //  A node is directly transformed into its directive-declaration
    //  and written to the pov file.
    //  Because the directive-declarations have to be at the top of the
    //  pov file, the hierachy is constructed in a string and then written
    //  to the end of the pov-file.
    //

    // Creates for _node a declare directive and writes it directly
    // to the pov file.
    // add a declare directive to the pov file to save this node information
    // and make it useable through an unique name ("D"+node pointer)
    add_declaration_to_pov_file(_node);

    // prepare the "use parent directive"
    std::string use_parent_directive = create_parent_directive(
        _node,
        keyword_map_[_node]);

    // construct the unions for all the children
    std::string children = unions_of_children(_node);

    // comment this out, to construct the hierachy like described above
    //return "union {\n" + children + declaration_usage + "}\n";


    // For a more compact representation of the scene graph,
    // use this:
    // Only a modifier node has to be enclosed in an union.

    // Modifier node
    if (is_modifier(_node))
    {
        // Modifier node without children will be removed.
        if (children == "")
            return "";

        // Is union unnecessary for the node and its children?
        if ( _node->parent_ != 0  &&
             is_modifier(_node->parent_) &&
             _node->parent_->children_.size() == 1)
        {
            return children + use_parent_directive;
        }
        else
        {
            return "union {\n" + children + use_parent_directive + "}\n";
        }
    }

    // Object node
    return children + use_parent_directive;

}


// ---------------------------------------------------------------------------


bool
Scene_graph_converter::
is_modifier(Base_node* _node)
{
    // Is _node modifier-node or perhaps not yet evaluated?
    bool check = keyword_map_[_node] == "transform" ||
                 keyword_map_[_node] == "texture"  ||
                 keyword_map_[_node] == "";

    return check;
}


// ---------------------------------------------------------------------------


}
