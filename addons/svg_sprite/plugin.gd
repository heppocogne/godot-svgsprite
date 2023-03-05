@tool
extends EditorPlugin

const _rawsvg_root:="res://_rawsvg"
const compress_setting:="svgsprite/compress"
var res_file_sys:EditorFileSystem
var file_sys:FileSystemDock
var _compress_setting_cache:bool


func _enter_tree():
	if Engine.is_editor_hint:
		res_file_sys=get_editor_interface().get_resource_filesystem()
		res_file_sys.connect("resources_reimported",Callable(self,"_on_resources_reimported"))
		file_sys=get_editor_interface().get_file_system_dock()
		file_sys.connect("file_removed",Callable(self,"_on_file_removed"))
		file_sys.connect("files_moved",Callable(self,"_on_files_moved"))
		connect("project_settings_changed",Callable(self,"_on_project_settings_changed"))
		
		var dir:=DirAccess.open("res://")
		if !dir.dir_exists(_rawsvg_root):
			dir.make_dir(_rawsvg_root)
			pick_svg_files("res://")
	
#	add_custom_type("SVGSprite",
#					"Node2D",
#					preload("res://addons/svg_sprite/svgsprite_library.gdns"),
#					preload("res://addons/svg_sprite/icon_svgsprite.svg"))
	
	if !ProjectSettings.has_setting(compress_setting):
		ProjectSettings.set_setting(compress_setting,false)
		
	_compress_setting_cache=ProjectSettings.get_setting(compress_setting)


func _exit_tree():
	remove_custom_type("SVGSprite")
	
	if Engine.is_editor_hint:
		res_file_sys.disconnect("resources_reimported",Callable(self,"_on_resources_reimported"))
		file_sys.disconnect("file_removed",Callable(self,"_on_file_removed"))
		file_sys.disconnect("files_moved",Callable(self,"_on_files_moved"))


func pick_svg_files(start_path:String):
	var dir:=DirAccess.open(start_path)
	dir.list_dir_begin()
	var item_name:=dir.get_next()
	var svg_files:PackedStringArray=[]
	
	while item_name!="":
		if item_name==".gdignore":
			break
		var item_path:=dir.get_current_dir()+"/"+item_name
		if dir.dir_exists(item_path):
			pick_svg_files(item_path)
		elif dir.file_exists(item_path) and item_name.ends_with(".svg"):
			svg_files.append(item_path)
		item_name=dir.get_next()
	dir.list_dir_end()
	
	_on_resources_reimported(svg_files)


func get_rawsvg_path(path:String)->String:
	return _rawsvg_root+"/"+path.get_file()+"-"+path.get_basename().sha256_text()+".rawsvg"


func get_rawsvgz_path(path:String)->String:
	return _rawsvg_root+"/"+path.get_file()+"-"+path.get_basename().sha256_text()+".rawsvgz"


func _on_resources_reimported(paths:PackedStringArray):
	for path in paths:
		if path.ends_with(".svg"):
			if ProjectSettings.get_setting(compress_setting):
				var f:FileAccess
				var svg:String
				f=FileAccess.open(path,FileAccess.READ)
				if f:
					svg=f.get_as_text()
					f.close()
				f=FileAccess.open_compressed(get_rawsvgz_path(path),FileAccess.WRITE,FileAccess.COMPRESSION_DEFLATE)
				if f:
					f.store_string(svg)
					f.close()
			else:
				var d:=DirAccess.copy_absolute(path,get_rawsvg_path(path))


func _on_file_removed(path:String):
	if path.ends_with(".svg"):
		if ProjectSettings.get_setting(compress_setting):
			DirAccess.remove_absolute(get_rawsvg_path(path))
		else:
			DirAccess.remove_absolute(get_rawsvgz_path(path))


func _on_files_moved(old_path:String,new_path:String):
	if old_path.ends_with(".svg"):
		if ProjectSettings.get_setting(compress_setting):
			DirAccess.rename_absolute(get_rawsvg_path(old_path),get_rawsvg_path(new_path))
		else:
			DirAccess.rename_absolute(get_rawsvgz_path(old_path),get_rawsvgz_path(new_path))


func _on_project_settings_changed():
	if _compress_setting_cache!=ProjectSettings.get_setting(compress_setting):
		var dir:=DirAccess.open(_rawsvg_root)
		dir.list_dir_begin()
		var item_name:=dir.get_next()
		while item_name!="":
			dir.remove(item_name)
			item_name=dir.get_next()
		dir.list_dir_end()
		
		pick_svg_files("res://")
		_compress_setting_cache=ProjectSettings.get_setting(compress_setting)
