import os
import sys

current_dir = os.path.dirname(__file__)
ecad_lib_path = os.path.abspath(current_dir + "/../build/")
sys.path.append(ecad_lib_path)

import PyEcad as ecad
from PyEcad import EPoint2D

###EDataMgr
def test_data_mgr() :
    #instance
    mgr = ecad.EDataMgr.instance()

    db_name = "test"
    #create database
    mgr.create_database(db_name)

    #open database
    database = mgr.open_database(db_name)
    assert(database != None)

    #remove database
    assert(mgr.remove_database(db_name))
    database = mgr.open_database(db_name)
    assert(database == None)

    #save database
    database = mgr.create_database(db_name)
    bin_file = current_dir + '/' + db_name + '.bin'
    if os.path.isfile(bin_file) :
        os.remove(bin_file)
    assert(mgr.save_database(database, bin_file))
    assert(os.path.isfile(bin_file) == True)
    xml_file = current_dir + '/' + db_name + '.xml'
    if os.path.isfile(xml_file) :
        os.remove(xml_file)
    assert(mgr.save_database(database, xml_file, ecad.EArchiveFormat.XML))
    assert(os.path.isfile(xml_file) == True)

    #load database
    assert(mgr.load_database(database, bin_file))
    assert(mgr.load_database(database, xml_file, ecad.EArchiveFormat.XML))
    os.remove(bin_file)
    os.remove(xml_file)

    #create cell
    cell_name = "cell"
    cell = mgr.create_circuit_cell(database, cell_name)
    assert(cell != None)

    #find cell
    found_cell = mgr.find_cell_by_name(database, cell_name)
    assert(found_cell != None)
    assert(found_cell.suuid == cell.suuid)

    #create net
    net_name = "net"
    layout = cell.get_layout_view()
    net = mgr.create_net(layout, net_name)
    assert(net != None)

    net_iter = layout.get_net_iter()
    next = net_iter.next()
    assert(next.name == net_name)
    next = net_iter.next()
    assert(next == None)

    #create stackup layer
    layer_m = mgr.create_stackup_layer("m", ecad.ELayerType.CONDUCTINGLAYER, 0, 10)
    assert(layer_m.thickness == 10)
    layer_v = mgr.create_stackup_layer("v", ecad.ELayerType.DIELECTRICLAYER, 0, 20)
    layer_v.elevation = -10
    assert(layer_v.elevation == -10)

    #create layer map
    layer_map_name = "layer_map"
    layer_map = mgr.create_layer_map(database, layer_map_name)
    assert(layer_map.get_database().suuid == database.suuid)

    #create padstack def
    padstack_def_name = "padstack_def"
    padstack_def = mgr.create_padstack_def(database, padstack_def_name)

    #create padstack def data
    padstackdef_data = mgr.create_padstack_def_data()

    #create padstack inst
    padstack_inst_name = "padstack_inst"
    padstack_inst_tran = ecad.ETransform2D()
    padstack_inst = mgr.create_padstack_inst(layout, padstack_inst_name, padstack_def, ecad.ENetId(-1), ecad.ELayerId(-1), ecad.ELayerId(-1), layer_map, padstack_inst_tran)
    assert(padstack_inst != None)

    #create cell inst
    sub_cell_name = "sub_cell"
    sub_cell = mgr.create_circuit_cell(database, sub_cell_name)
    sub_layout = sub_cell.get_layout_view()
    cell_inst_name = "cell_inst"
    cell_inst_tran = ecad.ETransform2D()
    cell_inst = mgr.create_cell_inst(layout, cell_inst_name, sub_layout, cell_inst_tran)
    assert(cell_inst != None)

    #create shape polygon
    points = [EPoint2D(0, 0), EPoint2D(10, 0), EPoint2D(10, 10), EPoint2D(0, 10)]
    eshape = mgr.create_shape_polygon(points)
    assert(eshape.shape.size() == 4)
    eshape2 = mgr.create_shape_polygon(ecad.EPolygonData())
    assert(eshape2.shape.size() == 0)

    #create shape polygon with holes
    eshape3 = mgr.create_shape_polygon_with_holes(ecad.EPolygonWithHolesData())
    assert(eshape3 != None)

    #create geometry 2d
    geom = mgr.create_geometry_2d(layout, ecad.ELayerId.NOLAYER, ecad.ENetId.NONET, eshape3)
    assert(geom != None)

    #create text
    s = "hello world"
    text = mgr.create_text(layout, ecad.ELayerId.NOLAYER, ecad.ETransform2D(), s)
    assert(text.text == s)

    #shut down
    mgr.shutdown(False)
    mgr.shutdown()

###EDatabase
def test_database() :

    #create database
    db_name = "test"
    database = ecad.EDatabase(db_name)

    #save
    bin_file = current_dir + '/' + db_name + '.bin'
    xml_file = current_dir + '/' + db_name + '.xml'
    if os.path.isfile(bin_file) :
        os.remove(bin_file)
    assert(database.save(bin_file))
    assert(os.path.isfile(bin_file) == True)
    if os.path.isfile(xml_file) :
        os.remove(xml_file)
    assert(database.save(xml_file, ecad.EArchiveFormat.XML))
    assert(os.path.isfile(xml_file) == True)

    #load
    assert(database.load(bin_file))
    assert(database.load(xml_file, ecad.EArchiveFormat.XML))
    os.remove(bin_file)
    os.remove(xml_file)

    #coord units
    coord_units = database.coord_units
    database.coord_units = coord_units

    #get next def name
    next_def_name = database.get_next_def_name("def", ecad.EDefinitionType.PADSTACKDEF)
    assert(next_def_name == "def")

    #get cell collection
    cell_collection = database.get_cell_collection()

    #create cell
    cell_name = "cell"
    cell = database.create_circuit_cell(cell_name)
    assert(cell != None)

    #find cell
    found_cell = database.find_cell_by_name(cell_name)
    assert(found_cell != None)
    assert(found_cell.suuid == cell.suuid)

    #get_circuit_cells
    circuit_cells = database.get_circuit_cells()
    assert(len(circuit_cells) == 1)
    assert(circuit_cells[0].name == cell_name)

    #get_top_cells
    top_cells = database.get_top_cells()
    assert(len(top_cells) == 1)
    
    #flatten
    assert(database.flatten(top_cells[0]))

    #get layer map collection
    layer_map_collection = database.get_layer_map_collection()

    #create layer map
    layer_map_name = "layer_map"
    layer_map = database.create_layer_map(layer_map_name)
    assert(layer_map != None)
    
    #add layer map
    assert(database.add_layer_map(layer_map) == False)

    #get padstack def collection
    padstack_def_collection = database.get_padstack_def_collection()

    #create padstack def
    padstack_def_name = "padstack"
    padstack_def = database.create_padstack_def(padstack_def_name)
    assert(padstack_def != None)
    assert(padstack_def.name == padstack_def_name)

    #get cell iter
    iterator = database.get_cell_iter()
    next = iterator.next()
    assert(next.name == cell_name)
    next = iterator.next()
    assert(next == None)

    #get layer map iter
    iterator = database.get_layer_map_iter()
    next = iterator.next()
    assert(next.name == layer_map_name)
    next = iterator.next()
    assert(next == None)

    #get padstack def iter
    iterator = database.get_padstack_def_iter()
    next = iterator.next()
    assert(next.name == padstack_def_name)
    next = iterator.next()
    assert(next == None)

    #name
    assert(database.name == db_name)

    #suuid
    suuid = database.suuid

    #clear
    database.clear()
    assert(cell_collection.size() == 0)
    assert(layer_map_collection.size() == 0)
    assert(padstack_def_collection.size() == 0)

###ECell
def test_cell() :
    #create database
    db_name = "test"
    database = ecad.EDatabase(db_name)

    #create cell
    cell_name = "cell"
    cell = database.create_circuit_cell(cell_name)

    #create layout
    layout_name = "layout"
    layout = ecad.ELayoutView(layout_name, None)

    #set layout view
    assert(cell.set_layout_view(layout))

    #get cell type
    assert(cell.get_cell_type() == ecad.ECellType.CIRCUITCELL)

    #get database
    assert(cell.get_database().suuid == database.suuid)

    #get layout view
    layout = cell.get_layout_view() 
    assert(layout != None)
    assert(layout.name == layout_name)
    assert(layout.get_cell().suuid == cell.suuid)

    #get flattened layout view
    flattened = cell.get_flattened_layout_view()
    assert(flattened != None)

    #get definition type
    assert(ecad.EDefinitionType.CELL == cell.get_definition_type())

    #name/suuid
    assert(cell_name == cell.name)
    assert(cell.suuid)

###ELayoutView
def test_layout_view() :

     #create database
    db_name = "test"
    database = ecad.EDatabase(db_name)

    #create circuit cell
    cell_name = "cell"
    cell = database.create_circuit_cell(cell_name)
    
    #get layout view
    layout = cell.get_layout_view()

    #name/suuid
    assert(layout.name == cell_name)
    assert(layout.suuid)

    #get net iter
    net_iter = layout.get_net_iter()

    #get layer iter
    layer_iter = layout.get_layer_iter()

    #get conn obj iter
    conn_obj_iter = layout.get_conn_obj_iter()

    #get primitive iter
    primitve_iter = layout.get_primitive_iter()

    #get hierarchy obj iter
    hierarchy_obj_iter = layout.get_hierarchy_obj_iter()

    #get padstask inst iter
    padstack_inst_iter = layout.get_padstack_inst_iter()

    #append layer
    layer = ecad.EStackupLayer("layer0", ecad.ELayerType.CONDUCTINGLAYER)
    assert(ecad.ELayerId(0) == layout.append_layer(layer))

    #append layers
    layers = [ ecad.EStackupLayer("layer" + str(i), ecad.ELayerType.CONDUCTINGLAYER) for i in range(1, 3)]
    assert(layout.append_layers(layers) == [1, 2])

    #get stackup layers
    layers = layout.get_stackup_layers()
    assert(len(layers) == 3)

    #add default dielectric layers
    layer_map = layout.add_default_dielectric_layers()
    assert(layer_map.get_mapping_forward(ecad.ELayerId(1)) == 2)
    assert(layer_map.get_mapping_backward(ecad.ELayerId(4)) == 2)

    #create net
    net_name = "net"
    net = layout.create_net(net_name)
    net_iter = layout.get_net_iter()
    assert(net.suuid == net_iter.next().suuid)

    #create padstack inst
    padstack_name = "padstack"
    padstack_def = ecad.EPadstackDef("padstackdef")
    padstack_inst = layout.create_padstack_inst(padstack_name, padstack_def, ecad.ENetId.NONET, ecad.ELayerId(0), ecad.ELayerId(1), database.create_layer_map("layer_map"), ecad.ETransform2D())
    padstack_inst_iter = layout.get_padstack_inst_iter()
    assert(padstack_inst.suuid == padstack_inst_iter.next().suuid)

    #create cell inst
    cell_name = "cell"
    cell = ecad.ECircuitCell(cell_name, None)
    cell_inst = layout.create_cell_inst(cell_name, cell.get_layout_view(), ecad.ETransform2D())
    cell_inst_iter = layout.get_cell_inst_iter()
    assert(cell_inst.suuid == cell_inst_iter.next().suuid)

    #create geometry 2d
    polygon_data = ecad.EPolygonData()
    polygon_data.set_points([EPoint2D(0, 0), EPoint2D(10, 0), EPoint2D(10, 10), EPoint2D(0, 10)])
    shape_polygon = ecad.EPolygon()
    shape_polygon.shape = polygon_data
    geom = layout.create_geometry_2d(ecad.ELayerId(0), ecad.ENetId.NONET, shape_polygon)
    assert(geom != None)
    geom_iter = layout.get_primitive_iter()
    assert(geom.suuid == geom_iter.next().suuid)

    #create text
    text = layout.create_text(ecad.ELayerId.NOLAYER, ecad.ETransform2D(), "text")
    assert(text != None)

    prim_iter = layout.get_primitive_iter()
    prim = prim_iter.next()
    while prim :
        t = prim.get_text_from_primitive()
        if t :
            assert(t.suuid == text.suuid)
        g = prim.get_geometry_2d_from_primitive()
        if g :
            assert(g.suuid == geom.suuid)
        prim = prim_iter.next()

    #get net collection
    net_collection = layout.get_net_collection()
    net = net_collection[net_name]
    assert(net.name == net_name)

    #get layer collection
    layer_collection = layout.get_layer_collection()
    assert(len(layer_collection) == 5)

    #get conn obj collection
    conn_obj_collection = layout.get_conn_obj_collection()
    assert(len(conn_obj_collection) == 3)

    #get cell inst collection
    cell_inst_collection = layout.get_cell_inst_collection()
    assert(len(cell_inst_collection) == 1)

    #get primitive collection
    primitive_collection = layout.get_primitive_collection()
    assert(len(primitive_collection) == 2)

    #get hierarchy obj collection
    hierarchy_obj_collection = layout.get_hierarchy_obj_collection()
    assert(len(hierarchy_obj_collection) == 1)

    #get padstack inst collection
    padstack_inst_collection = layout.get_padstack_inst_collection()
    assert(len(padstack_inst_collection) == 1)

    #set boundary
    boundary = ecad.EPolygon()
    boundary.set_points([EPoint2D(0, 0), EPoint2D(10, 0), EPoint2D(10, 10), EPoint2D(0, 10)])
    layout.set_boundary(boundary)

    #get boundary
    boundary = layout.get_boundary()

    #generate metal fraction mapping
    mf_settings = ecad.EMetalFractionMappingSettings()
    assert(layout.generate_metal_fraction_mapping(mf_settings))

    #connectivity extraction
    layout.connectivity_extraction()

    #flatten
    layout.flatten(ecad.EFlattenOption())

    #merge
    layout.merge(layout, ecad.ETransform2D())

    #map
    layout.map(layer_map)

###ECellCollection
def test_cell_collection() :
    #create database
    db_name = "test"
    database = ecad.EDatabase(db_name)

    #create circuit cell
    r = range(0, 1)
    for i in r :
        database.create_circuit_cell('cell_{}'.format(i + 1))
    
    cell_collection = database.get_cell_collection()
    #__len__/size
    assert(len(cell_collection) == 1)
    assert(cell_collection.size() == 1)

    #__getitem__/get cell iter
    assert(cell_collection['cell_1'].suuid == cell_collection.get_cell_iter().next().suuid)

    #__contains__
    assert(('cell_0' in cell_collection) == False)

    #clear
    cell_collection.clear()
    assert(len(cell_collection) == 0)

###EPoint
def test_point() :
    p = EPoint2D(2, 3)
    assert(p.x == 2 and p.y == 3)
    p.x = 3
    p.y = 2
    assert(p.x == 3 and p.y == 2)

###ETransform
def test_transform() :
    trans = ecad.make_transform_2d(0.5, 0, EPoint2D(3, 5), True)
    m = trans.get_transform()
    assert(m.a11 == -.5 and m.a13 == 3.0 and m.a22 == 0.5 and m.a23 == 5.0)

###EPolygonData
def test_polygon_data() :
    points = [EPoint2D(0, 0), EPoint2D(10, 0), EPoint2D(10, 10), EPoint2D(0, 10)]
    polygon = ecad.EPolygonData()
    polygon.set_points(points)
    assert(polygon.size() == 4)

def main() :
    test_data_mgr()
    test_database()
    test_cell()
    test_layout_view()
    test_cell_collection()
    test_point()
    test_transform()
    test_polygon_data()
    print("every thing is fine")

if __name__ == '__main__' :
    main()