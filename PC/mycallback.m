
function mycallback(hObject, eventdata, BT, handles)
global sensor_data;
global map_links;
id_byte = fread(BT,1);
disp(id_byte)
switch id_byte
    case 1 %sensorvärden
        collect_and_plot_sensor_data(BT, handles);
    case 3 %distance
        distance = fread(BT,1);
        set(handles.distance,'String',distance)
    case 8 %auto_decisions
        collect_and_display_steering_commands(BT, handles);
    case 12 %Map links
        collect_and_plot_map_links(BT, handles)
    case 13 %Map parameters
        %disp('update map')
        collect_and_display_map_parameters(BT, handles);
    otherwise
        disp('error i switch för id-byte')
end



function collect_and_plot_sensor_data(BT, handles)
global sensor_data;
new_sensor_data = fread(BT,7);
new_IR_sensor_data = new_sensor_data(3:7);
sensor_data = [sensor_data, new_IR_sensor_data]; %lägger till ett element sist i listan
sensor_data(:,1) = []; %tar bort det första elementet i listan
%%Kommentera in nedanstående två rader om du vill plotta sensordata och
%%kommentera ut dem om du vill plotta kartan
%figure(handles.sensor_and_map_plot);
%plot(sensor_data'); 
set(handles.error, 'string', new_sensor_data(1))
set(handles.angle, 'string', new_sensor_data(2))
set(handles.ir_f, 'string', new_sensor_data(3))
set(handles.ir_h_b, 'string', new_sensor_data(4))
set(handles.ir_h_f, 'string', new_sensor_data(5))
set(handles.ir_v_b, 'string', new_sensor_data(6))
set(handles.ir_v_f, 'string', new_sensor_data(7))

function collect_and_plot_map_links(BT, handles)
global map_links;
new_link = fread(BT, 4);
map_links = [map_links; new_link'];
size(map_links, 1);
% %%Kommentera bort nedanstående forloop samt hold on och hold off om du 
% %%vill plotta sensordata och kommentera in dem om du vill plotta kartan
hold on
for j = 1:size(map_links, 1)
    X1 = map_links(j,1);
    Y1 = map_links(j,2);
    X2 = map_links(j,3);
    Y2 = map_links(j,4);
    figure(handles.sensor_and_map_plot);
    line([X1 X2], [Y1 Y2]);
    plot([X1 X2], [Y1 Y2], 'o')
end
minx = min(min(map_links(:,[1 3]))); %minsta värdet i alla rader kolumn 1 och 3
miny = min(min(map_links(:,[2 4])));
maxx = max(max(map_links(:,[1 3])));
maxy = max(max(map_links(:,[2 4])));
axis([minx-1 maxx+1 miny-1 maxy+1]);
hold off



function collect_and_display_steering_commands(BT, handles)
set(handles.styrform, 'String', 'Autonom')
byte = fread(BT,1);
%disp(byte)
switch byte
    case 16 %Stop
        set(handles.kommando, 'String', 'Stanna')
        drawnow() %Tvingar GUI att uppdatera sig behövs den?
    case 17 %Forward
        set(handles.kommando, 'String', 'Fram')
        drawnow()
    case 18 %Backward
        set(handles.kommando, 'String', 'Back')
        drawnow()
    case 19 %Turn Right
        set(handles.kommando, 'String', 'Sväng höger')
        drawnow()
    case 20 %Turn left
        set(handles.kommando, 'String', 'Sväng vänster')
        drawnow()
    case 21 %Rotate Right
        set(handles.kommando, 'String', 'Rotera höger')
        drawnow()
    case 22 %Rotera vänster
        set(handles.kommando, 'String', 'Rotera vänster')
        drawnow()
    case 23 %Close claw
        set(handles.kommando, 'String', 'Greppa med gripklo')
        drawnow()
    case 24 %Open claw
        set(handles.kommando, 'String', 'öppna gripklo')
        drawnow()
    case 25 %Forward not regulated
        set(handles.kommando, 'String', 'Fram, ej reglerad')
        drawnow()
    case 26 %Backward not regulated
        set(handles.kommando, 'String', 'Back, ej reglerad')
        drawnow()
    case 27 %Tight turn right
        set(handles.kommando, 'String', 'Snäv högersväng')
        drawnow()
    case 28 %Tight turn left
        set(handles.kommando, 'String', 'Snäv vänstersväng')
        drawnow()
    otherwise
        disp('error i auto_decisions')
end

function collect_and_display_map_parameters(BT, handles)
    map_parameters = fread(BT,7);
    set(handles.robot_direction,'String',map_parameters(1));
    drawnow() %Testa om det behöver vara drawnow() mellan varje rad.
    set(handles.size_all_nodes,'String',map_parameters(2));
    drawnow()
    set(handles.following_path,'String',map_parameters(3));
    drawnow()
    set(handles.enable_node_edit,'String',map_parameters(4));
    drawnow()
    set(handles.level,'String',map_parameters(5));
    drawnow()
    set(handles.x_coordinate,'String',map_parameters(6));
    drawnow()
    set(handles.y_coordinate,'String',map_parameters(7));
    drawnow()
    