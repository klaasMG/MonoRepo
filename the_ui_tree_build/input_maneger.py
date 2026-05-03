import queue
import print_wrapper
from pynput import keyboard, mouse
from enum import Enum
from typing import Callable, Any
from GSGwidget import GSGWidget

class FocusManager:
    def __init__(self):
        self.focused_widget = GSGWidget(parent=-1)
        self.focused_widget.id = -1

    def set_focused_widget(self, widget):
        self.focused_widget = widget

    def get_focused_widget(self):
        return self.focused_widget.id

class ActionType(Enum):
    MoveMouse = 0
    MousePress = 1
    MouseRelease = 2
    Scroll = 3
    KeyPress = 4
    KeyRelease = 5

class Action:
    def __init__(self, action_type: ActionType, action_data):
        self.action_type: ActionType = action_type
        self.data = action_data

class Keys(Enum):
    A = "a"
    B = "b"
    C = "c"
    D = "d"
    E = "e"
    F = "f"
    G = "g"
    H = "h"
    I = "i"  # noqa: E741
    J = "j"
    K = "k"
    L = "l"
    M = "m"
    N = "n"
    O = "o"  # noqa: E741
    P = "p"
    Q = "q"
    R = "r"
    S = "s"
    T = "t"
    U = "u"
    V = "v"
    W = "w"
    X = "x"
    Y = "y"
    Z = "z"

    ZERO = "0"
    ONE = "1"
    TWO = "2"
    THREE = "3"
    FOUR = "4"
    FIVE = "5"
    SIX = "6"
    SEVEN = "7"
    EIGHT = "8"
    NINE = "9"

    GRAVE = "`"
    MINUS = "-"
    EQUAL = "="
    LEFT_BRACKET = "["
    RIGHT_BRACKET = "]"
    BACKSLASH = "\\"
    SEMICOLON = ";"
    APOSTROPHE = "'"
    COMMA = ","
    PERIOD = "."
    SLASH = "/"

    SPACE = "Key.space"
    ENTER = "Key.enter"
    TAB = "Key.tab"
    BACKSPACE = "Key.backspace"
    ESC = "Key.esc"
    CAPS_LOCK = "Key.caps_lock"

    SHIFT = "Key.shift"
    SHIFT_L = "Key.shift_l"
    SHIFT_R = "Key.shift_r"

    CTRL = "Key.ctrl"
    CTRL_L = "Key.ctrl_l"
    CTRL_R = "Key.ctrl_r"

    ALT = "Key.alt"
    ALT_L = "Key.alt_l"
    ALT_R = "Key.alt_r"

    CMD = "Key.cmd"
    CMD_L = "Key.cmd_l"
    CMD_R = "Key.cmd_r"

    UP = "Key.up"
    DOWN = "Key.down"
    LEFT = "Key.left"
    RIGHT = "Key.right"

    INSERT = "Key.insert"
    DELETE = "Key.delete"
    HOME = "Key.home"
    END = "Key.end"
    PAGE_UP = "Key.page_up"
    PAGE_DOWN = "Key.page_down"
pynput_to_key_mapping: dict[str, Keys] = {
    "a": Keys.A,
    "b": Keys.B,
    "c": Keys.C,
    "d": Keys.D,
    "e": Keys.E,
    "f": Keys.F,
    "g": Keys.G,
    "h": Keys.H,
    "i": Keys.I,
    "j": Keys.J,
    "k": Keys.K,
    "l": Keys.L,
    "m": Keys.M,
    "n": Keys.N,
    "o": Keys.O,
    "p": Keys.P,
    "q": Keys.Q,
    "r": Keys.R,
    "s": Keys.S,
    "t": Keys.T,
    "u": Keys.U,
    "v": Keys.V,
    "w": Keys.W,
    "x": Keys.X,
    "y": Keys.Y,
    "z": Keys.Z,

    "0": Keys.ZERO,
    "1": Keys.ONE,
    "2": Keys.TWO,
    "3": Keys.THREE,
    "4": Keys.FOUR,
    "5": Keys.FIVE,
    "6": Keys.SIX,
    "7": Keys.SEVEN,
    "8": Keys.EIGHT,
    "9": Keys.NINE,

    "`": Keys.GRAVE,
    "-": Keys.MINUS,
    "=": Keys.EQUAL,
    "[": Keys.LEFT_BRACKET,
    "]": Keys.RIGHT_BRACKET,
    "\\": Keys.BACKSLASH,
    ";": Keys.SEMICOLON,
    "'": Keys.APOSTROPHE,
    ",": Keys.COMMA,
    ".": Keys.PERIOD,
    "/": Keys.SLASH,

    "Key.space": Keys.SPACE,
    "Key.enter": Keys.ENTER,
    "Key.tab": Keys.TAB,
    "Key.backspace": Keys.BACKSPACE,
    "Key.esc": Keys.ESC,
    "Key.caps_lock": Keys.CAPS_LOCK,

    "Key.shift": Keys.SHIFT,
    "Key.shift_l": Keys.SHIFT_L,
    "Key.shift_r": Keys.SHIFT_R,

    "Key.ctrl": Keys.CTRL,
    "Key.ctrl_l": Keys.CTRL_L,
    "Key.ctrl_r": Keys.CTRL_R,

    "Key.alt": Keys.ALT,
    "Key.alt_l": Keys.ALT_L,
    "Key.alt_r": Keys.ALT_R,

    "Key.cmd": Keys.CMD,
    "Key.cmd_l": Keys.CMD_L,
    "Key.cmd_r": Keys.CMD_R,

    "Key.up": Keys.UP,
    "Key.down": Keys.DOWN,
    "Key.left": Keys.LEFT,
    "Key.right": Keys.RIGHT,

    "Key.insert": Keys.INSERT,
    "Key.delete": Keys.DELETE,
    "Key.home": Keys.HOME,
    "Key.end": Keys.END,
    "Key.page_up": Keys.PAGE_UP,
    "Key.page_down": Keys.PAGE_DOWN,
}
class Buttons(Enum):
    LEFT = "Button.left"
    RIGHT = "Button.right"
    MIDDLE = "Button.middle"

    X1 = "Button.x1"
    X2 = "Button.x2"
pynput_to_mouse_mapping: dict[str, Buttons] = {
    "Button.left": Buttons.LEFT,
    "Button.right": Buttons.RIGHT,
    "Button.middle": Buttons.MIDDLE,

    "Button.x1": Buttons.X1,
    "Button.x2": Buttons.X2,
}

class InputManager:
    def __init__(self):
        self.mouse_listener = mouse.Listener(
            on_move=self.on_move,
            on_click=self.on_click,
            on_scroll=self.on_scroll
        )
        
        self.keyboard_listener = keyboard.Listener(
            on_press=self.on_press,
            on_release=self.on_release
        )
        self.ui_event_queue = queue.SimpleQueue()
        self.mouse_listener.start()
        self.keyboard_listener.start()

    def get_event(self)-> None | Action:
        try:
            return self.ui_event_queue.get_nowait()
        except queue.Empty:
            return None

    def is_event_available(self) -> bool:
        return self.ui_event_queue.qsize() > 0

    def on_move(self, x, y):
        self.ui_event_queue.put(Action(ActionType.MoveMouse, (x, y)))

    def on_click(self, x, y, button, pressed):
        if pressed:
            action_type = ActionType.MousePress
        else:
            action_type = ActionType.MouseRelease
        action: Action = Action(action_type, (x, y, button))
        self.convert_button_to_string_or_buttons(button)
        self.ui_event_queue.put(action)

    def on_scroll(self, x, y, dx, dy):
        self.ui_event_queue.put(Action(ActionType.Scroll, (x, y, dx, dy)))

    def on_press(self, key):
        key = self.convert_key_to_string_or_keys(key)
        self.ui_event_queue.put(Action(ActionType.KeyPress, key))

    def on_release(self, key):
        key = self.convert_key_to_string_or_keys(key)
        self.ui_event_queue.put(Action(ActionType.KeyRelease, key))

    @staticmethod
    def convert_key_to_string_or_keys(key):
        key_as_str = str(key).replace("'", "")
        try:
            key = pynput_to_key_mapping[key_as_str]
        except KeyError:
            key = key_as_str
        return key

    @staticmethod
    def convert_button_to_string_or_buttons(button):
        button_as_str = str(button).replace("'", "")
        try:
            button = pynput_to_mouse_mapping[button_as_str]
        except KeyError:
            button = button_as_str
        return button

class InputRegistry:
    def __init__(self):
        self.registry: dict[tuple[Keys | Buttons | str | None, int, ActionType], dict[Callable, Any]] = {}

    def register_action(self, action: ActionType, widget_id:int, func: Callable, rules: Any, key: Keys | Buttons | str | None):
        self.registry[key, widget_id, action] = {func : rules}

    def change_rules(self, widget_id:int, action: ActionType,func: Callable, rules: Any, key: Keys | Buttons | str | None):
        self.registry[key, widget_id, action][func] = rules

    def remove_action(self, widget_id: int, action: ActionType, key: Keys | Buttons | str | None):
        del self.registry[key, widget_id, action]

    def remove_func(self, widget_id: int, action: ActionType, func: Callable, key: Keys | Buttons | str | None):
        del self.registry[key, widget_id, action][func]

    def register_func(self, widget_id: int, action: ActionType, func: Callable, rules: Any, key: Keys | Buttons | str | None):
        functions = self.registry[key, widget_id, action]
        functions[func] = rules
        self.registry[key, widget_id, action] = functions

    def lookup(self, widget_id: int, action: ActionType, key: Keys | Buttons | str | None):
        func_focus = self.registry[key, widget_id, action]
        return func_focus

    def is_already_registered(self, widget_id: int, action: ActionType, key: Keys | Buttons | str | None):
        return (key, widget_id, action) in self.registry