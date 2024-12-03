import tkinter as tk
from tkinter import filedialog, messagebox, ttk
from tkinter.scrolledtext import ScrolledText
import os


class XMLViewerApp:
    def __init__(self, root):
        self.root = root
        self.root.title("XML Viewer, Consistency Checker, and JSON Converter")
        self.root.geometry("900x700")
        self.root.configure(bg="#f5f5f5")

        # Title
        self.title_label = tk.Label(
            self.root,
            text="XML Viewer, Consistency Checker, and JSON Converter",
            font=("Arial", 16, "bold"),
            bg="#f5f5f5",
            fg="#333",
        )
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

        self.load_button = tk.Button(
            self.buttons_frame,
            text="Load XML File",
            command=self.load_xml_file,
            font=("Arial", 12),
            bg="#008000",
            fg="#fff",
            padx=10,
            pady=5,
        )
        self.load_button.grid(row=0, column=0, padx=5)

        self.check_button = tk.Button(
            self.buttons_frame,
            text="Check Consistency",
            command=self.check_consistency,
            font=("Arial", 12),
            bg="#007acc",
            fg="#fff",
            padx=10,
            pady=5,
        )
        self.check_button.grid(row=0, column=1, padx=5)

        self.json_button = tk.Button(
            self.buttons_frame,
            text="Convert to JSON",
            command=self.convert_to_json,
            font=("Arial", 12),
            bg="#007acc",
            fg="#fff",
            padx=10,
            pady=5,
        )
        self.json_button.grid(row=0, column=2, padx=5)

        # JSON Output Textbox
        self.json_output = ScrolledText(self.root, wrap=tk.WORD, height=15, font=("Arial", 10))
        self.json_output.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

        # Result Label
        self.result_label = tk.Label(self.root, text="", font=("Arial", 14, "italic"), bg="#f5f5f5", fg="#333")
        self.result_label.pack(pady=10)

        self.file_path = None

    def load_xml_file(self):
        self.file_path = filedialog.askopenfilename(filetypes=[("XML Files", "*.xml")])
        if self.file_path:
            try:
                with open(self.file_path, 'r') as file:
                    xml_content = file.read()
                self.result_label.config(text="XML Loaded Successfully.", fg="green")
                self.json_output.delete(1.0, tk.END)
                self.json_output.insert(tk.END, xml_content)
            except Exception as e:
                messagebox.showerror("Error", f"Failed to load XML file: {e}")

    def check_consistency(self):
        if not self.file_path:
            messagebox.showwarning("Warning", "Please load an XML file first.")
            return

        try:
            with open(self.file_path, 'r') as file:
                xml_content = file.read()

            # Calculate character count
            character_count = len(xml_content)

            # Check XML consistency
            is_consistent = self.check_xml_consistency(xml_content)

            # Update result label with both character count and consistency status
            result_text = f"XML Consistency: {'Consistent' if is_consistent else 'Inconsistent'}\n"
            result_text += f"Number of Characters: {character_count}"
            self.result_label.config(text=result_text, fg="green" if is_consistent else "red")

        except Exception as e:
            messagebox.showerror("Error", f"An error occurred: {e}")


    def convert_to_json(self):
        if not self.file_path:
            messagebox.showwarning("Warning", "Please load an XML file first.")
            return

        try:
            with open(self.file_path, 'r') as file:
                xml_content = file.read()
            is_consistent = self.check_xml_consistency(xml_content)
            if not is_consistent:
                messagebox.showwarning("Warning", "XML is inconsistent. Conversion to JSON skipped.")
                return
            json_output = self.convert_xml_to_json(xml_content)
            self.json_output.delete(1.0, tk.END)
            self.json_output.insert(tk.END, json_output)
            self.result_label.config(text="JSON Conversion Successful.", fg="green")
            output_file = os.path.join(os.getcwd(), "output.json")
            with open(output_file, 'w') as json_file:
                json_file.write(json_output)
            messagebox.showinfo("Success", f"JSON saved to {output_file}")
        except Exception as e:
            messagebox.showerror("Error", f"An error occurred: {e}")

    def check_xml_consistency(self, xml_content):
        stack = []
        i = 0
        while i < len(xml_content):
            if xml_content[i] == '<':
                if xml_content[i + 1] == '/':  # Closing tag
                    i += 2
                    closing_tag = ''
                    while i < len(xml_content) and xml_content[i] != '>':
                        closing_tag += xml_content[i]
                        i += 1
                    if not stack or stack[-1] != closing_tag:
                        return False
                    stack.pop()
                else:  # Opening tag
                    i += 1
                    opening_tag = ''
                    while i < len(xml_content) and xml_content[i] != '>':
                        opening_tag += xml_content[i]
                        i += 1
                    stack.append(opening_tag)
            i += 1
        return len(stack) == 0

    def convert_xml_to_json(self, xml_content):
        from xml.etree.ElementTree import ElementTree, fromstring
        try:
            tree = ElementTree(fromstring(xml_content))
            root = tree.getroot()

            def element_to_dict(element):
                node = {
                    "tag": element.tag,
                    "text": element.text.strip() if element.text else "",
                    "children": [element_to_dict(child) for child in element]
                }
                return node

            json_structure = element_to_dict(root)

            import json
            return json.dumps(json_structure, indent=2)
        except Exception as e:
            raise RuntimeError(f"Error during XML to JSON conversion: {e}")


if __name__ == "__main__":
    root = tk.Tk()
    app = XMLViewerApp(root)
    root.mainloop()
