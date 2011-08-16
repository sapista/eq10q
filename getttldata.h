///TODO: this is a start point to get values from .ttl files in exectuion time
//Be sure to set the LV2 search path to your develop location in order to do tests and debug, also you can link the actual search path to your develop dir

//This functions are form slv2 library, check the documentation

//Get world
SLV2World 	slv2_world_new (void)

//Loads a bundle
void 	slv2_world_load_bundle (SLV2World world, SLV2Value bundle_uri)

//Get plugin object by URL
SLV2Plugin 	slv2_world_get_plugin_by_uri_string (SLV2World world, const char *uri)

//Get the Value by string
SLV2Value slv2_value_new_string (SLV2World  world, const char *str) 	
	
//Get a port object
SLV2Port 	slv2_plugin_get_port_by_symbol (SLV2Plugin plugin, SLV2Value symbol)

//Get port range for a given port and plugin
void 	slv2_port_get_range (SLV2Plugin plugin, SLV2Port port, SLV2Value *deflt, SLV2Value *min, SLV2Value *max)


//When finished of
void slv2_world_free 	( 	SLV2World  	world	) 	