# Simple Banner Module

There are settings in the module you can alter.
The module will make a Banner node.
It will allow you to updated a Banner image to that node, and relate it to other nodes of type
1. page
2. webform

Feel free to add more.
(no admin interface in order to keep settings in config)
@todo make the config separate from the git repo like a yml file.

Then when you edit those Nodes you can change the relationship it has to that banner as well.

The relationship goes
Many banner to one Node.

# MVC

This was a try at MVC and it was an early try so not that great.

### Model.inc

This will output the banner info based on the node id or banner id

### View.inc

Different outputs 

### Controller.inc

This has, for some reasons, the forms as well (@todo move this to View.inc)
And some of the crud.



