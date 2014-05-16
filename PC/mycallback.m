 
function mycallback(hObject, eventdata, BT, sensor_data, number_of_sensor_data_collected, handles)

 %load('data_från_robot') %load och savwe fungerar, men känns som om det
 %blir trögt..
 id_byte = fread(BT,1);
 %disp(id_byte)
 switch id_byte 
   case 1 %sensorvärden
         for j = 1:7
             %Ev lägga in en while här, men känns onödigt om felmeddelandet
             %aldrig uppstår
            byte = fread(BT,1);
             switch j
                 case 1
                     sensor_data(j, number_of_sensor_data_collected) = byte;
                     set(handles.error,'String',byte)
                     drawnow()
                 case 2
                     sensor_data(j, number_of_sensor_data_collected) = byte;
                     set(handles.angle,'String',byte)
                     drawnow()
                case 3
                    sensor_data(j, number_of_sensor_data_collected) = byte;
                    set(handles.ir_f,'String',byte)
                    drawnow()
                case 4
                    sensor_data(j, number_of_sensor_data_collected) = byte;
                    set(handles.ir_h_b,'String',byte)
                    drawnow()
                case 5
                    sensor_data(j, number_of_sensor_data_collected) = byte;
                    set(handles.ir_h_f,'String',byte)
                    drawnow()
                case 6
                    sensor_data(j, number_of_sensor_data_collected) = byte;
                    set(handles.ir_v_b,'String',byte)
                    drawnow()
                case 7
                    sensor_data(j, number_of_sensor_data_collected) = byte;
                    set(handles.ir_v_f,'String',byte)
                    drawnow()
                otherwise 
                    disp('error i switch för sensordata')
            end
         end
     number_of_sensor_data_collected = number_of_sensor_data_collected + 1;
    case 3 %distance
        byte = fread(BT,1);
        set(handles.distance,'String',byte)
    case 8 %auto_decisions
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
    case 12 %Map coordinates
        disp('uppdate map')
        x_coordinate = fread(BT, 1);
        y_coordinate = fread(BT, 1);
        coordinates = [x_coordinate, y_coordinate];
        disp(coordinates)
    case 13 %Map parameters
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
                    disp('error i switch för kartparametrar')
          end
        end
    otherwise
        disp('error i switch för id-byte')
end

% start_value_to_plot_array = 1;
% if(size(IR_front_long)> 100)
%     start_value_to_plot_array = size(IR_front_long) - 100;
% end
% axes(handles.ir_front_long)
% plot(IR_front_long(start_value_to_plot_array:size(IR_front_long)))
% axes(handles.ir_right_back)
% plot(IR_right_back(start_value_to_plot_array:size(IR_right_back)))
% axes(handles.ir_right_front)
% plot(IR_right_front(start_value_to_plot_array:size(IR_right_front)))
% axes(handles.ir_left_back)
% plot(IR_left_back(start_value_to_plot_array:size(IR_right_front)))
% axes(handles.ir_left_front)
% plot(IR_left_front(start_value_to_plot_array:size(IR_left_front)))
% drawnow()

% disp(IR_left_front)
% disp(IR_left_back)

% disp(IR_right_front)
% disp(IR_right_back)
% disp(IR_front_long)
% disp(regulator_angle)
% disp(regulator_error)
%save('data_från_robot')
%disp('all data du ville hämta är hämtad')