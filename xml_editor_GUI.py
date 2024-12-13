import ttkbootstrap as ttk
from ttkbootstrap.constants import *
from tkinter import filedialog, messagebox
import tkinter as tk
import subprocess
import os


class XMLGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("XML Editor GUI")
        self.root.geometry("800x600")

        style = ttk.Style(theme="superhero")

        input_frame = ttk.Frame(root, padding=10)
        input_frame.pack(fill=X)

        self.input_file_label = ttk.Label(input_frame, text="Input File:", font=("Helvetica", 12, "bold"))
        self.input_file_label.pack(side=LEFT, padx=5)

        self.input_file_path = ttk.Entry(input_frame, width=50, font=("Helvetica", 10))
        self.input_file_path.pack(side=LEFT, padx=5, fill=X, expand=True)

        self.browse_button = ttk.Button(input_frame, text="Browse", bootstyle=INFO, command=self.browse_file)
        self.browse_button.pack(side=LEFT, padx=5)

        # Create a notebook with tabs for output and file content
        output_frame = ttk.Frame(root, padding=10)
        output_frame.pack(fill=BOTH, expand=True)

        notebook = ttk.Notebook(output_frame)
        notebook.pack(fill=BOTH, expand=True, padx=5, pady=5)

        # Output log tab
        self.output_log_frame = ttk.Frame(notebook, padding=10)
        notebook.add(self.output_log_frame, text="Operation Log")

        self.output_text = ttk.Text(self.output_log_frame, height=15, width=70, font=("Consolas", 10), state="disabled")
        self.output_text.pack(fill=BOTH, expand=True)

        # File content tab
        self.file_content_frame = ttk.Frame(notebook, padding=10)
        notebook.add(self.file_content_frame, text="File Content")

        self.file_content_text = ttk.Text(self.file_content_frame, height=15, width=70, font=("Consolas", 10), state="disabled")
        self.file_content_text.pack(fill=BOTH, expand=True)

        # Operations Frame
        operations_frame = ttk.Frame(root, padding=10)
        operations_frame.pack(fill=X)

        button_width = 20
        buttons = [
            ("Verify XML", self.verify_xml, SUCCESS),
            ("Fix Errors and Save", self.fix_errors, WARNING),
            ("Format XML", self.format_xml, PRIMARY),
            ("Convert to JSON", self.convert_to_json, INFO),
            ("Minify XML", self.minify_xml, DARK),
            ("Compress Data", self.compress_data, SECONDARY),
            ("Decompress Data", self.decompress_data, LIGHT),
            ("Save Output", self.save_output, SUCCESS),
        ]

        for col, (text, command, style) in enumerate(buttons):
            button = ttk.Button(operations_frame, text=text, width=button_width, bootstyle=style, command=command)
            button.grid(row=0, column=col, padx=5, pady=5)

        self.output_content = ""

        self.xml_editor_path = "D:/Koleyaaaaa/Senior 1/Data Structures and Algorithms/Old Project/Level 1/xml_editor.exe"

    def display_output(self, stdout, stderr):
        self.output_text.config(state="normal")
        self.output_text.delete(1.0, tk.END)

        if stderr:
            self.output_text.insert(tk.END, "Error:\n" + stderr + "\n")
        if stdout:
            self.output_text.insert(tk.END, "Output:\n" + stdout + "\n")
            self.output_content = stdout

        self.output_text.config(state="disabled")

    def display_file_content(self, content):
        self.file_content_text.config(state="normal")
        self.file_content_text.delete(1.0, tk.END)
        self.file_content_text.insert(tk.END, content)
        self.file_content_text.config(state="disabled")

    def browse_file(self):
        file_path = filedialog.askopenfilename(
            filetypes=[("All Supported Files", "*.xml *.comp"), ("XML files", "*.xml"), ("Compressed files", "*.comp")]
        )
        if file_path:
            self.input_file_path.delete(0, tk.END)
            self.input_file_path.insert(0, file_path)
            # Display the selected file content
            try:
                with open(file_path, "r") as file:
                    content = file.read()
                self.display_file_content(content)
            except Exception as e:
                messagebox.showerror("Error", f"Failed to display file content: {e}")

    def verify_xml(self):
        self.run_xml_editor_command("verify")

    def fix_errors(self):
        self.run_xml_editor_command("verify", extra_args=["-f"])

    def format_xml(self):
        self.run_xml_editor_command("format")

    def convert_to_json(self):
        self.run_xml_editor_command("json", output_extension=".json")
        # Load and display the converted JSON content
        try:
            with open(self.output_file, "r") as f:
                content = f.read()
            self.display_file_content(content)
        except Exception as e:
            messagebox.showerror("Error", f"Failed to display file content: {e}")

    def minify_xml(self):
        self.run_xml_editor_command("mini")

    def compress_data(self):
        self.run_xml_editor_command("compress", output_extension=".comp")

    def decompress_data(self):
        input_file = self.input_file_path.get()
        if not input_file.endswith(".comp"):
            messagebox.showerror("Error", "Input file must be a .comp file for decompression!")
            return

        output_file = filedialog.asksaveasfilename(defaultextension=".xml", filetypes=[("XML files", "*.xml")])
        if not output_file:
            return

        if not os.path.exists(self.xml_editor_path):
            messagebox.showerror("Error", "The xml_editor executable was not found! Ensure it is compiled and in the correct directory.")
            return

        args = [self.xml_editor_path, "decompress", "-i", input_file, "-o", output_file]

        try:
            process = subprocess.run(args, capture_output=True, text=True)
            self.display_output(process.stdout, process.stderr)

            # Display the decompressed file content in the "File Content" tab
            if os.path.exists(output_file):
                try:
                    with open(output_file, "r") as file:
                        content = file.read()
                    self.display_file_content(content)
                except Exception as e:
                    messagebox.showerror("Error", f"Failed to display file content: {e}")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to execute decompression: {e}")

    def save_output(self):
        if not self.output_content:
            messagebox.showerror("Error", "No output to save!")
            return

        output_file = filedialog.asksaveasfilename(defaultextension=".txt",
                                                   filetypes=[("Text files", "*.txt")])
        if output_file:
            try:
                with open(output_file, "w") as file:
                    file.write(self.output_content)
                messagebox.showinfo("Success", f"Output saved to {output_file}")
            except Exception as e:
                messagebox.showerror("Error", f"Failed to save output: {e}")

    def run_xml_editor_command(self, command, extra_args=None, output_extension=".xml"):
        input_file = self.input_file_path.get()
        if not os.path.exists(input_file):
            messagebox.showerror("Error", "Input file does not exist!")
            return

        output_file = None
        if command != "verify":
            output_file = filedialog.asksaveasfilename(
                defaultextension=output_extension,
                filetypes=[(f"{output_extension.upper()} files", f"*{output_extension}")]
            )
            if not output_file:
                return

        if not os.path.exists(self.xml_editor_path):
            messagebox.showerror("Error", "The xml_editor executable was not found! Ensure it is compiled and in the correct directory.")
            return

        args = [self.xml_editor_path, command, "-i", input_file]
        if output_file:
            args.extend(["-o", output_file])
        if extra_args:
            args.extend(extra_args)

        try:
            process = subprocess.run(args, capture_output=True, text=True)
            self.display_output(process.stdout, process.stderr)

            # Display the output file content in the "File Content" tab
            if output_file and os.path.exists(output_file):
                try:
                    with open(output_file, "r") as file:
                        content = file.read()
                    self.display_file_content(content)
                except Exception as e:
                    messagebox.showerror("Error", f"Failed to display file content: {e}")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to execute {command}: {e}")


if __name__ == "__main__":
    root = ttk.Window(themename="superhero")
    app = XMLGUI(root)    
    root.mainloop()
