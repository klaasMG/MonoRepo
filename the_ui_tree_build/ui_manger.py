import sys
import time
import numpy as np
from print_wrapper import tprint, dbg
from renderer import GSGRenderSystem
from PySide6.QtWidgets import QApplication
from PySide6.QtCore import QTimer
from pathlib import Path
from FontManager import FontManager
from input_maneger import InputManager, Action, ActionType, InputRegistry, FocusManager, Keys, Buttons
from typing import Any, Callable
from widget_data import WidgetDataType
from event_system import event_system, EventQueue, EventTypeEnum
from threading import Lock, Thread
from update_data_manager import DataHolder
from GSGwidget import GSGWidget
from hold_lock import HoldLock

class app(QApplication):
    def __init__(self, *args, event_system = None,parent = None, **kwargs):
        super().__init__(*args , **kwargs)
        self.event_system = event_system
        self.parent_stop = parent
        self.aboutToQuit.connect(self.on_quit)
    
    def on_quit(self):
        tprint("Application is quitting")
        if self.event_system is not None:
            self.event_system.stop_event_system()
            self.parent_stop.running = False

class GSGUiManager:
    def __init__(self):
        self.focus_manager = FocusManager()
        self.input_registry = InputRegistry()
        self.buffers_swapped = False
        self.square_exist = False
        self.write_widget_data = Lock()
        self.depth_layers = 100
        self.widget_data = {}
        self.widget_max = 10000
        self.init_widget_data(widget_data_types={WidgetDataType.POSITION : (self.widget_max * 6 , np.int32),
                                                 WidgetDataType.SHADER_PASS : (self.widget_max , np.int32) ,
                                                 WidgetDataType.COLOUR : (self.widget_max * 4 , np.int32) ,
                                                 WidgetDataType.SHAPE : (self.widget_max , np.int32) ,
                                                 WidgetDataType.ASSETS_ID : (self.widget_max , np.int32) ,
                                                 WidgetDataType.TEXT_ID : (self.widget_max , np.int32) ,
                                                 WidgetDataType.PARENT : (self.widget_max , np.int32)})
        self.widgets_by_id = {}
        self.free_ids = []
        self.next_id = 0
        self.GSG_renderer_system: None | GSGRenderSystem = None
        self.input_manager: InputManager = InputManager()
        self.hold_lock = HoldLock()
        self.Widget_update_data = DataHolder(self)
        self.window_top = None
        self.window_bottom = None
        self.capture_input = False
        self.ui_manager_queue: EventQueue = event_system.add_queue("ui_manager")
        self.mouse_pos_queue: EventQueue = event_system.add_queue("mouse_pos")
        self.assets = []
        self.text = []
        self.text_ids: dict[tuple[str, int], int] = {}
        self.asset_ids = {}
        self.next_text_id = 0
        self.next_asset_id = 0
        self.text_set = set()
        self.asset_path = set()
        self.root = GSGWidget(0)
        self.append_widget(self.root, data=None)
        self.width = 0
        self.height = 0
        self.app = app(sys.argv, event_system=event_system, parent=self)
        self.running = True
        self.widget_thread = Thread(target=self.update_widgets)
        ttf_path = Path("assets/fonts/AovelSansRounded-rdDL.ttf")
        self.font_manager = FontManager()
        self.font_manager.add_font("Font", ttf_path)
    
    def run_ui_manager(self):
        self.GSG_renderer_system = GSGRenderSystem(self)
        self.GSG_renderer_system.show()
        self.widget_thread.start()
        self.frame_timer = QTimer()
        self.frame_timer.timeout.connect(self.update_ui_manager)
        self.frame_timer.start(16)  # ~60 FPS

        sys.exit(self.app.exec())

    
    def update_ui_manager(self):
        self.ui_manager_queue.receive_event()
        if self.GSG_renderer_system is not None:
            self.GSG_renderer_system.render_update()
        
    def use_event(self, event):
        event_type, data = event
        if event_type == 255:
            return None
        if event_type == EventTypeEnum.Resize:
            self.width = data[0]
            self.height = data[1]
        return None
    
    def update_widgets(self):
        while self.running:
            events: list[Action] = self.get_input_events(max_event_requests=100)
            for event in events:
                action_type: ActionType = event.action_type
                event_data = event.data
                for i in event_data:
                    print(i)
                if action_type == ActionType.MousePress or action_type == ActionType.MouseRelease:
                    position_x: int = event_data[0]
                    position_y: int = event_data[1]
                    height, widget_id = self.GSG_renderer_system.last_frame.get_pixel_data(position_x, position_y)
                    is_input_registered: bool = self.input_registry.is_already_registered(widget_id, action_type, event_data[2])
                    if is_input_registered:
                        self.focus_manager.set_focused_widget(widget_id)
                widget_id = self.focus_manager.get_focused_widget()
                button = None
                if action_type == ActionType.MouseRelease or action_type == ActionType.MousePress:
                    button = event_data[2]
                elif action_type == ActionType.KeyRelease or action_type == ActionType.KeyPress:
                    button = event_data
                if self.input_registry.is_already_registered(widget_id, action_type, button):
                    func: Callable = self.input_registry.lookup(widget_id, action_type, button)
                    func()
            if not self.square_exist:
                self.sqaure = GSGWidget(parent=self.root)
                path_or_data = "yes"
                self.append_widget(self.sqaure, {WidgetDataType.POSITION: [320, 200, 1, 420, 300, 1],
                                             WidgetDataType.COLOUR: [255, 255, 25, 255], WidgetDataType.SHADER_PASS: 2,
                                             WidgetDataType.SHAPE: -1,
                                             WidgetDataType.PATH_OR_DATA: path_or_data,
                                             WidgetDataType.ASSET_OR_TEXT: "text"})
                def print_key_press_happend():
                    print("key_press_happened")
                self.add_input_event_to_widget(self.sqaure, ActionType.KeyPress, {}, print_key_press_happend, Keys.L)
                self.square_exist = True
            time.sleep(0.1)

    def get_input_events(self, max_event_requests: int | None = None):
        events: list[Action] = []
        max_event_requests_local = max_event_requests
        while self.input_manager.is_event_available() and (max_event_requests_local is None or isinstance(max_event_requests_local, int)):
            if max_event_requests_local is not None:
                if max_event_requests_local < 0:
                    break
            event: Action = self.input_manager.get_event()
            events.append(event)
            if max_event_requests_local is not None:
                max_event_requests_local -= 1
        return events
    
    def append_widget(self , widget, data):
        if self.free_ids:
            widget.id = self.free_ids.pop()
        else:
            widget.id = self.next_id
            self.next_id += 1
        self.widgets_by_id[widget.id] = widget
        self.set_default_widget_data(widget, data=data, is_new=True)
        
    def set_widget_data(self, widget, data,is_new):
        self.Widget_update_data.update_widget_data(widget, data, is_new)
    
    def set_default_widget_data(self, widget, data: dict,is_new):
        if data is not None:
            used = set(data.keys())
            expected = set(WidgetDataType)
            excluded = {WidgetDataType.ASSETS_ID, WidgetDataType.ASSETS, WidgetDataType.TEXT, WidgetDataType.TEXT_ID, WidgetDataType.PARENT, WidgetDataType.TEXT_BOXES}
        
            missing = expected - excluded - used
            if missing:
                raise ValueError(f"Missing enum cases: {missing}")
            self.set_widget_data(widget, data,is_new)
    
    def update_widget(self , widget_id,parent , data=None):
        if not data or len(data) != 14 or data == [-1] * 14:
            return
        pos = data[0:6]
        col = data[6:10]
        for p , j in enumerate(pos):
            if j == -1:
                pos[p] = self.widget_data[WidgetDataType.POSITION][widget_id * 6 + p]
        for p , j in enumerate(col):
            if j == -1:
                col[p] = self.widget_data[WidgetDataType.COLOUR][widget_id * 4 + p]
        self.widget_data[WidgetDataType.POSITION][widget_id * 6:widget_id * 6 + 6] = pos
        self.widget_data[WidgetDataType.COLOUR][widget_id * 4:widget_id * 4 + 4] = col
        self.widget_data[WidgetDataType.SHADER_PASS][widget_id] = data[10] if data[10] != -1 else self.widget_data[WidgetDataType.SHADER_PASS][widget_id]
        self.widget_data[WidgetDataType.SHAPE][widget_id] = data[11] if data[11] != -1 else self.widget_data[WidgetDataType.SHAPE][widget_id]
        self.widget_data[WidgetDataType.PARENT][widget_id] = parent if parent != -1 else self.widget_data[WidgetDataType.PARENT][widget_id]
        if data[13] == "text":
            text: str = data[12]
            key = (text, pos[4] - pos[1])
            if key not in self.text_ids:
                self.text_ids[key] = self.next_text_id
                self.text.append(text)
                dbg(f"{text} -> {self.next_text_id}")
                self.widget_data[WidgetDataType.TEXT_ID][widget_id] = self.next_text_id
                text_heigt: int = key[1]
                self.font_manager.render_text(text, "Font", text_heigt, self.next_text_id)
                self.next_text_id += 1
            else:
                id = self.text_ids[key]
                self.widget_data[WidgetDataType.TEXT_ID][widget_id] = id
        elif data[13] == "asset":
            self.widget_data[WidgetDataType.ASSETS_ID][widget_id] = self.next_asset_id
            self.asset_ids[data[12]] = self.next_asset_id
            self.next_asset_id += 1
    
    def set_widget_defaults(self , widget_id, parent , data = None):
        if not data or len(data) != 14:
            data = [-1] * 14
        pos = data[0:6]
        col = data[6:10]
        self.widget_data[WidgetDataType.POSITION][widget_id * 6:widget_id * 6 + 6] = pos
        self.widget_data[WidgetDataType.COLOUR][widget_id * 4:widget_id * 4 + 4] = col
        self.widget_data[WidgetDataType.SHADER_PASS][widget_id] = data[10]
        self.widget_data[WidgetDataType.SHAPE][widget_id] = data[11]
        self.widget_data[WidgetDataType.PARENT][widget_id] = parent if parent is not None else -1
        if data[13] == "text":
            text: str = data[12]
            for pose in pos:
                print(pose)
            key = (text, pos[4] - pos[1])
            if key not in self.text_ids:
                self.text_ids[key] = self.next_text_id
                self.text.append(text)
                dbg(f"{text} -> {self.next_text_id}")
                self.widget_data[WidgetDataType.TEXT_ID][widget_id] = self.next_text_id
                dbg(f"{self.widget_data[WidgetDataType.TEXT_ID][widget_id]} -> {self.next_text_id}")
                text_heigt: int = key[1]
                print(f"this is the text?{text} {text_heigt} {self.next_text_id}")
                self.font_manager.render_text(text, "Font", text_heigt, self.next_text_id)
                self.next_text_id += 1
            else:
                id = self.text_ids[key]
                self.widget_data[WidgetDataType.TEXT_ID][widget_id] = id
        elif data[13] == "asset":
            self.widget_data[WidgetDataType.ASSETS_ID][widget_id] = self.next_asset_id
            self.asset_ids[data[12]] = self.next_asset_id
            self.next_asset_id += 1
    
    def clear_widget_data(self , wid):
        default = -1
        self.widget_data[WidgetDataType.POSITION][wid * 6:wid * 6 + 6] = [default , default , default , default , default , default]
        self.widget_data[WidgetDataType.COLOUR][wid * 4:wid * 4 + 4] = [default , default , default , default]
        self.widget_data[WidgetDataType.SHADER_PASS][wid] = default
        self.widget_data[WidgetDataType.SHAPE][wid] = default
        self.widget_data[WidgetDataType.ASSETS_ID][wid] = default
        self.widget_data[WidgetDataType.TEXT_ID][wid] = default
        self.widget_data[WidgetDataType.PARENT][wid] = default

    def add_input_event_to_widget(self, widget: GSGWidget , action_type: ActionType, rules: Any, func: Callable, key_button: Keys | None | Buttons | str = None):
        widget_id = widget.id
        if not self.input_registry.is_already_registered(widget_id, action_type, key_button):
            self.input_registry.register_action(action_type, widget_id, func, rules, key_button)
        else:
            self.input_registry.register_func(widget_id, action_type, func, rules, key_button)

    def remove_input_event_from_widget(self, widget: GSGWidget , action_type: ActionType, func: Callable, key_button: Keys | None | Buttons | str = None):
        widget_id = widget.id
        self.input_registry.remove_func(widget_id, action_type, func, key_button)

    def remove_widget_input_events(self, widget: GSGWidget , action_type: ActionType, key_button: Keys | None | Buttons | str = None):
        widget_id = widget.id
        self.input_registry.remove_action(widget_id, action_type, key_button)

    def change_func_rules_for_widget(self, widget: GSGWidget, action: ActionType,func: Callable, rules: Any, key_button: Keys | None | Buttons | str = None):
        widget_id = widget.id
        self.input_registry.change_rules(widget_id, action,func, rules, key_button)
    
    def init_widget_data(self , widget_data_types: dict):
        for key , (size , dtype) in widget_data_types.items():
            arr = np.full(size , -1 , dtype=dtype)
            self.widget_data[key] = arr
        
    def add_asset(self,path):
        self.asset_path.add(path)
            
    def pos_update(self):
        if self.GSG_renderer_system is not None:
            acquired = self.hold_lock.lock(time_out=0.01)
            if acquired:
                self.GSG_renderer_system.widget_data, self.widget_data = self.widget_data, self.GSG_renderer_system.widget_data
                self.buffers_swapped = True
                self.hold_lock.release()

if __name__ == "__main__":
    manager = GSGUiManager()
    manager.run_ui_manager()