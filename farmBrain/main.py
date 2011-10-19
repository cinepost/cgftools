import gtk, webkit, os

w = gtk.Window()
w.set_title("Brain")
w.connect("destroy", gtk.main_quit)

editor = webkit.WebView()
editor.load_html_string("<p>This is a <b>test</b>", "file:///")
editor.set_editable(True)

def on_action(action):
	editor.execute_script("document.execCommand('%s', false, false);" % action.get_name())

actions = gtk.ActionGroup("Actions")
actions.add_actions([
  ("bold", gtk.STOCK_BOLD, "_Bold", "<ctrl>B", None, on_action),
  ("italic", gtk.STOCK_ITALIC, "_Italic", "<ctrl>I", None, on_action),
  ("underline", gtk.STOCK_UNDERLINE, "_Underline", "<ctrl>U", None, on_action),
])

ui_def = """
<toolbar name="toolbar_format">
  <toolitem action="bold" />
  <toolitem action="italic" />
  <toolitem action="underline" />
</toolbar>
"""

ui = gtk.UIManager()
ui.insert_action_group(actions)
ui.add_ui_from_string(ui_def)

vb = gtk.VBox()
vb.pack_start(ui.get_widget("/toolbar_format"), False)
vb.pack_start(editor, True)

w.add(vb)
w.show_all()

gtk.main()
