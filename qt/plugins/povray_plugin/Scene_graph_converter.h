#ifndef GRAPHENE_SCENE_GRAPH_CONVERTER_H
#define GRAPHENE_SCENE_GRAPH_CONVERTER_H

#include <graphene/gl/gl_includes.h>
#include <graphene/scene_graph/Scene_graph.h>
#include <graphene/scene_graph/Base_node.h>

#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <sstream>

using namespace  graphene::scene_graph;

namespace graphene {


// converts a scene graph into the Povray files format
class Scene_graph_converter
{
public:

    // maps a node to its usage specifier.
    // the usage specifier is used in the hierarchy
    // to apply a nodes declaration
    typedef std::map<Base_node*,std::string> Key_word_type;

    // contains all options for Povray
    struct Povray_options
    {
        // povray files name prefix
        std::string name_prefix;
        // picture width
        int width;
        // picture height
        int height;
        // quality setting used by Povray
        int quality;
        // sampling method used by Povray
        int sampling_method;
        // depth setting used by Povray
        int depth;
        // threshold setting used by Povray
        double threshold;
        // jitter setting used by Povray
        double jitter;
        // antialias setting used by Povray
        bool antialias;
        // verbose flag used by Povray
        bool verbose;
        // display flag used by Povray
        bool display;
    };


public:

    // Constructor expects scene graph and Povray options
    // that should be used.
    Scene_graph_converter(const Scene_graph* _graph,
                          const Povray_options _options
                          );

    // starts conversion from scene graph to povray file format
    bool start();


private:

    // creates .pov file for the scene graph
    void create_pov_file();
    // creates .ini file for the scene graph
    void create_ini_file();

    // traverese scene graph
    std::string traverse(Base_node* _node);

    // creates recursively the scene graph hierarchy
    // for the sub tree beginning with _node.
    // aka: creates all the unions for _nodes children
    // and their declarations
    std::string unions_of_children(Base_node* _node);

    // add a declare directive to save this node information
    // under and make it useable through an unique name
    void add_declaration_to_pov_file(Base_node* _node);

    // check, if node is modifier (Material_node, ...)
    bool is_modifier(Base_node* _node);


private:

    // used scene graph
    const Scene_graph* graph_;

    // saves the Povray options
    const Povray_options options_;

    // output streams for the pov- and ini-file
    std::ofstream pov_file_;
    std::ofstream ini_file_;

    // list of all unique nodes in the scene graph
    std::vector<Base_node*> nodes_;

    // keeps track which node declarations should be used in
    // the hierarchy and in which way
    Key_word_type keyword_map_;


};

}

#endif
