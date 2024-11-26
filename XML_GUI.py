import tkinter as tk
from tkinter import filedialog, messagebox, ttk
import subprocess
import xml.etree.ElementTree as ET


class XMLViewerApp:
    def __init__(self, root):
        self.root = root
        self.root.title("XML File Viewer")
        self.root.geometry("900x700")
        self.root.configure(bg="#f5f5f5")

        # Title
        self.title_label = tk.Label(self.root, text="XML Viewer & Consistency Checker", font=("Arial", 16, "bold"), bg="#f5f5f5", fg="#333")
        self.title_label.pack(pady=10)

        # Treeview Frame
        self.treeview_frame = tk.Frame(self.root, bg="#f5f5f5")
        self.treeview_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=5)

        # Scrollbars
        self.treeview_scrollbar_y = tk.Scrollbar(self.treeview_frame, orient=tk.VERTICAL)
        self.treeview_scrollbar_y.pack(side=tk.RIGHT, fill=tk.Y)

        self.treeview_scrollbar_x = tk.Scrollbar(self.treeview_frame, orient=tk.HORIZONTAL)
        self.treeview_scrollbar_x.pack(side=tk.BOTTOM, fill=tk.X)

        # Treeview Widget
        self.treeview = ttk.Treeview(
            self.treeview_frame,
            columns=("Attributes"),
            yscrollcommand=self.treeview_scrollbar_y.set,
            xscrollcommand=self.treeview_scrollbar_x.set,
        )
        self.treeview.heading("#0", text="Tag")
        self.treeview.heading("Attributes", text="Attributes / Text")
        self.treeview.column("#0", width=200)
        self.treeview.column("Attributes", width=400)
        self.treeview.pack(fill=tk.BOTH, expand=True)

        self.treeview_scrollbar_y.config(command=self.treeview.yview)
        self.treeview_scrollbar_x.config(command=self.treeview.xview)

        # Buttons
        self.buttons_frame = tk.Frame(self.root, bg="#f5f5f5")
        self.buttons_frame.pack(pady=10)

        self.load_button = tk.Button(self.buttons_frame, text="Load XML File", command=self.load_xml_file, font=("Arial", 12), bg="#007acc", fg="#fff", padx=10, pady=5)
        self.load_button.grid(row=0, column=0, padx=5)

        self.check_button = tk.Button(self.buttons_frame, text="Check Consistency", command=self.check_consistency, font=("Arial", 12), bg="#007acc", fg="#fff", padx=10, pady=5)
        self.check_button.grid(row=0, column=1, padx=5)

        # Result Label
        self.result_label = tk.Label(self.root, text="", font=("Arial", 14, "italic"), bg="#f5f5f5", fg="#333")
        self.result_label.pack(pady=10)

    def load_xml_file(self):
        file_path = filedialog.askopenfilename(filetypes=[("XML Files", "*.xml")])
        if file_path:
            try:
                tree = ET.parse(file_path)
                root = tree.getroot()
                self.treeview.delete(*self.treeview.get_children())
                self.display_xml_tree(root)
                self.file_path = file_path
                self.result_label.config(text="XML Loaded Successfully.", fg="green")
            except ET.ParseError as e:
                messagebox.showerror("Error", f"Failed to parse XML file: {e}")
            except Exception as e:
                messagebox.showerror("Error", f"An unexpected error occurred: {e}")

    def display_xml_tree(self, xml_element, parent=""):
        node = self.treeview.insert(parent, "end", text=xml_element.tag, values=(self.format_attributes(xml_element),))
        for child in xml_element:
            self.display_xml_tree(child, node)

    def format_attributes(self, element):
        if element.attrib:
            return ', '.join([f'{key}="{value}"' for key, value in element.attrib.items()])
        elif element.text and element.text.strip():
            return element.text.strip()
        return "No Content"

    def check_consistency(self):
        if not hasattr(self, "file_path") or not self.file_path:
            messagebox.showwarning("No File", "Please load an XML file first.")
            return

        try:
            result = subprocess.run(
                ["./XML_Consistency", self.file_path],
                capture_output=True,
                text=True,
            )
            if result.returncode == 0:
                self.result_label.config(
                    text=f"XML Consistency: {result.stdout.strip()}", fg="green"
                )
            else:
                self.result_label.config(
                    text="Error during consistency check.", fg="red"
                )
        except Exception as e:
            messagebox.showerror("Error", f"Failed to run consistency checker: {e}")


if __name__ == "__main__":
    root = tk.Tk()
    app = XMLViewerApp(root)
    root.mainloop()
