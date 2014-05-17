
function mycallback(hObject, eventdata, BT, handles)
global sensor_data;
id_byte = fread(BT,1);
disp(id_byte)
switch id_byte
    case 1 %sensorv�rden
        collect_and_plot_sensor_data(BT, handles);
    case 3 %distance
        distance = fread(BT,1);
        set(handles.distance,'String',distance)
    case 8 %auto_decisions
        collect_and_display_steering_commands(BT, handles);
    case 12 %Map links
        %collect_and_plot_map_links(BT, handles)
    case 13 %Map parameters
        disp('update map')
        collect_and_display_map_parameters(BT, handles);
    otherwise
        disp('error i switch f�r id-byte')
end



function collect_and_plot_sensor_data(BT, handles)
global sensor_data;
new_sensor_data = fread(BT,7);
new_IR_sensor_data = new_sensor_data(3:7);
sensor_data = [sensor_data, new_IR_sensor_data]; %l�gger till ett element sist i listan
sensor_data(:,1) = []; %tar bort det f�rsta elementet i listan
figure(handles.sensor_plot);
plot(sensor_data');
set(handles.error, 'string', new_sensor_data(1))
set(handles.angle, 'string', new_sensor_data(2))
set(handles.ir_f, 'string', new_sensor_data(3))
set(handles.ir_h_b, 'string', new_sensor_data(4))
set(handles.ir_h_f, 'string', new_sensor_data(5))
set(handles.ir_v_b, 'string', new_sensor_data(6))
set(handles.ir_v_f, 'string', new_sensor_data(7))



function collect_and_plot_map_links(BT, handles)
new_link = fread(BT, 4);
handles.map_links = [handles.map_links; new_link];
hold on
for j = 1:size(handles.map_links, 1)
    X1 = handles.map_links(j,1);
    Y1 = handles.map_links(j,2);
    X2 = handles.map_links(j,3);
    Y2 = handles.map_links(j,4);
    line([X1 X2], [Y1 Y2]);
    figure(handles.map_plot);
    plot([X1 X2], [Y1 Y2], 'o')
end
minx = min(min(handles.map_links(:,[1 3]))); %minsta v�rdet i alla rader kolumn 1 och 3
miny = min(min(handles.map_links(:,[2 4])));
maxx = max(max(handles.map_links(:,[1 3])));
maxy = max(max(handles.map_links(:,[2 4])));
axis([minx-1 maxx+1 miny-1 maxy+1]);
hold off



function collect_and_display_steering_commands(BT, handles)
set(handles.styrform, 'String', 'Autonom')
byte = fread(BT,1);
%disp(byte)
switch byte
    case 16 %Stop
        set(handles.kommando, 'String', 'Stanna')
        drawnow() %Tvingar GUI att uppdatera sig
    case 17 %Forward
        set(handles.kommando, 'String', 'Fram')
        drawnow()
    case 18 %Backward
        set(handles.kommando, 'String', 'Back')
        drawnow()
    case 19 %Turn Right
        set(handles.kommando, 'String', 'Sv�ng h�ger')
        drawnow()
    case 20 %Turn left
        set(handles.kommando, 'String', 'Sv�ng v�nster')
        drawnow()
    case 21 %Rotate Right
        set(handles.kommando, 'String', 'Rotera h�ger')
        drawnow()
    case 22 %Rotera v�nster
        set(handles.kommando, 'String', 'Rotera v�nster')
        drawnow()
    case 23 %Close claw
        set(handles.kommando, 'String', 'Greppa med gripklo')
        drawnow()
    case 24 %Open claw
        set(handles.kommando, 'String', '�ppna gripklo')
        drawnow()
    case 25 %Forward not regulated
        set(handles.kommando, 'String', 'Fram, ej reglerad')
        drawnow()
    case 26 %Backward not regulated
        set(handles.kommando, 'String', 'Back, ej reglerad')
        drawnow()
    case 27 %Tight turn right
        set(handles.kommando, 'String', 'Sn�v h�gersv�ng')
        drawnow()
    case 28 %Tight turn left
        set(handles.kommando, 'String', 'Sn�v v�nstersv�ng')
        drawnow()
    otherwise
        disp('error i auto_decisions')
end

function collect_and_display_map_parameters(BT, handles)
for k = 1:7
    byte = fread(BT,1);
    switch k
        case 1
            set(handles.robot_direction,'String',byte)
            drawnow()
        case 2
            set(handles.size_all_nodes,'String',byte)
            drawnow()
        case 3
            set(handles.following_path,'String',byte)
            drawnow()
        case 4
            set(handles.enable_node_edit,'String',byte)
            drawnow()
        case 5
            set(handles.level,'String',byte)
            drawnow()
        case 6
            set(handles.x_coordinate,'String',byte)
            drawnow()
        case 7
            set(handles.y_coordinate,'String',byte)
            drawnow()
        otherwise
            disp('error i switch f�r kartparametrar')
    end
end