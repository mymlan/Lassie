%  function mycallback(obj, event, handles)
%  disp('test')
 %id_byte = fread(handles.BT,1);
 %disp(id_byte)
%  IR_front_long = zeros(1,1000);
% IR_right_front = zeros(1,1000);
% IR_right_back = zeros(1,1000);
% IR_left_front = zeros(1,1000);
% IR_left_back = zeros(1,1000);
% regulator_error = zeros(1,1000);
% regulator_angle = zeros(1,1000);
% 
% disp('bytes available')
% disp(handles.BT.BytesAvailable)
% 
% id_byte = fread(handles.BT,1);
% disp(id_byte)
% switch id_byte 
%   case 1 %sensorv�rden
%         for j = 1:7
%             %Ev l�gga in en while h�r, men k�nns on�digt om felmeddelandet
%             %aldrig uppst�r
%            byte = fread(handles.BT,1);
%             switch j
%                 case 1
%                     regulator_error(i) = byte;
%                     set(handles.error,'String',byte)
%                     drawnow()
%                 case 2
%                     regulator_angle(i) = byte;
%                     set(handles.angle,'String',byte)
%                     drawnow()
%                 case 3
%                     IR_front_long(i) = byte;
%                     set(handles.ir_f,'String',byte)
%                     drawnow()
%                 case 4
%                     IR_right_back(i) = byte;
%                     set(handles.ir_h_f,'String',byte)
%                     drawnow()
%                 case 5
%                     IR_right_front(i) = byte;
%                     set(handles.ir_h_b,'String',byte)
%                     drawnow()
%                 case 6
%                     IR_left_back(i) = byte;
%                     set(handles.ir_v_b,'String',byte)
%                     drawnow()
%                 case 7
%                     IR_left_front(i) = byte;
%                     set(handles.ir_v_f,'String',byte)
%                     drawnow()
%                 otherwise 
%                     disp('error i switch f�r sensordata')
%             end
%         end
%     case 3 %distance
%         byte = fread(handles.BT,1);
%         set(handles.distance,'String',byte)
%     case 8 %auto_decisions
%         set(handles.styrform, 'String', 'Autonom')
%         byte = fread(handles.BT,1);
%         %disp(byte)
%         switch byte
%             case 16 %Stop
%                 set(handles.kommando, 'String', 'Stanna')
%                 drawnow() %Tvingar GUI att uppdatera sig
%             case 17 %Forward
%                 set(handles.kommando, 'String', 'Fram')
%                 drawnow()
%             case 18 %Backward
%                 set(handles.kommando, 'String', 'Back')
%                 drawnow()
%             case 19 %Turn Right
%                 set(handles.kommando, 'String', 'Sv�ng h�ger')
%                 drawnow()
%             case 20 %Turn left
%                 set(handles.kommando, 'String', 'Sv�ng v�nster')
%                 drawnow()
%             case 21 %Rotate Right
%                 set(handles.kommando, 'String', 'Rotera h�ger')
%                 drawnow()
%             case 22 %Rotera v�nster
%                 set(handles.kommando, 'String', 'Rotera v�nster')
%                 drawnow()
%             case 23 %Close claw
%                 set(handles.kommando, 'String', 'Greppa med gripklo')
%                 drawnow()
%             case 24 %Open claw
%                 set(handles.kommando, 'String', '�ppna gripklo')
%                 drawnow()
%             case 25 %Forward not regulated
%                 set(handles.kommando, 'String', 'Fram, ej reglerad')
%                 drawnow()
%             case 26 %Backward not regulated
%                 set(handles.kommando, 'String', 'Back, ej reglerad')
%                 drawnow()
%             case 27 %Tight turn right
%                 set(handles.kommando, 'String', 'Sn�v h�gersv�ng')
%                 drawnow()   
%             case 28 %Tight turn left
%                 set(handles.kommando, 'String', 'Sn�v v�nstersv�ng')
%                 drawnow() 
%             otherwise
%                 disp('error i auto_decisions')
%         end
%     case 12 %Map coordinates
%         disp('uppdate map')
%         %Hur vill vi representera kartan
%         %x_coordinate = fread(handles.BT,1);
%         %y_coordinate = fread(handles.BT,1);
%     case 13 %Map parameters
%         for k = 1:7
%           byte = fread(handles.BT,1);
%           switch k
%                 case 1
%                     set(handles.robot_direction,'String',byte)
%                     drawnow()
%                 case 2
%                     set(handles.size_all_nodes,'String',byte)
%                     drawnow()
%                 case 3
%                     set(handles.following_path,'String',byte)
%                     drawnow()
%                 case 4
%                     set(handles.enable_node_edit,'String',byte)
%                     drawnow()
%                 case 5
%                     set(handles.level,'String',byte)
%                     drawnow()
%                 case 6
%                     set(handles.x_coordinate,'String',byte)
%                     drawnow()
%                 case 7
%                     set(handles.y_coordinate,'String',byte)
%                     drawnow()
%                 otherwise 
%                     disp('error i switch f�r kartparametrar')
%           end
%         end
%     otherwise
%         disp('error i switch f�r id-byte')
% end
% 
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
% 
% % disp(IR_left_front)
% % disp(IR_left_back)
% % disp(IR_right_front)
% % disp(IR_right_back)
% % disp(IR_front_long)
% % disp(regulator_angle)
% % disp(regulator_error)
% disp('all data du ville h�mta �r h�mtad')