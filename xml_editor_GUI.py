import ttkbootstrap as ttk
from ttkbootstrap.constants import *
from tkinter import filedialog, messagebox, simpledialog
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

        # XML operations
        xml_operations = [
            ("Verify XML", self.verify_xml, SUCCESS),
            ("Fix Errors and Save", self.fix_errors, WARNING),
            ("Format XML", self.format_xml, PRIMARY),
            ("Convert to JSON", self.convert_to_json, INFO),
            ("Minify XML", self.minify_xml, DARK),
            ("Compress Data", self.compress_data, SECONDARY),
            ("Decompress Data", self.decompress_data, LIGHT),
        ]

        # Graph operations
        graph_operations = [
            ("Draw Graph", lambda: self.run_xml_editor_command("draw"), INFO),
            ("Most Active", lambda: self.run_xml_editor_command("most_active"), SUCCESS),
            ("Most Influencer", lambda: self.run_xml_editor_command("most_influencer"), PRIMARY),
            ("Mutual Followers", self.run_mutual_followers, DARK),
            ("Suggest Followers", self.run_suggest_followers, WARNING),
            ("Search Posts", self.run_search_posts, SECONDARY),
        ]

        # Add XML operation buttons
        xml_frame = ttk.LabelFrame(operations_frame, text="XML Operations", padding=10, bootstyle=INFO)
        xml_frame.pack(fill=X, pady=5)
        for col, (text, command, style) in enumerate(xml_operations):
            button = ttk.Button(xml_frame, text=text, width=button_width, bootstyle=style, command=command)
            button.grid(row=0, column=col, padx=5, pady=5)

        # Add Graph operation buttons
        graph_frame = ttk.LabelFrame(operations_frame, text="Graph Operations", padding=10, bootstyle=SUCCESS)
        graph_frame.pack(fill=X, pady=5)
        for col, (text, command, style) in enumerate(graph_operations):
            button = ttk.Button(graph_frame, text=text, width=button_width, bootstyle=style, command=command)
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

            if os.path.exists(output_file):
                try:
                    with open(output_file, "r") as file:
                        content = file.read()
                    self.display_file_content(content)
                except Exception as e:
                    messagebox.showerror("Error", f"Failed to display file content: {e}")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to execute decompression: {e}")

    def run_mutual_followers(self):
        user_ids = simpledialog.askstring("Input", "Enter user IDs (comma-separated):")
        if user_ids:
            self.run_xml_editor_command("mutual", extra_args=["-ids", user_ids])

    def run_suggest_followers(self):
        user_id = simpledialog.askstring("Input", "Enter user ID:")
        if user_id:
            self.run_xml_editor_command("suggest", extra_args=["-id", user_id])

    def run_search_posts(self):
        search_type = simpledialog.askstring("Input", "Enter search type ('word' or 'topic'):")
        search_term = simpledialog.askstring("Input", f"Enter search {search_type}:")
        if search_type and search_term:
            extra_args = [f"-{search_type[0]}", search_term]
            self.run_xml_editor_command("search", extra_args=extra_args)

    def run_xml_editor_command(self, command, extra_args=None, output_extension=".xml"):
        input_file = self.input_file_path.get()
        if not os.path.exists(input_file):
            messagebox.showerror("Error", "Input file does not exist!")
            return

        # Determine if the command requires an output file
        commands_requiring_output = ["draw", "compress", "decompress", "format", "json", "mini"]
        output_file = None

        # If the command is "draw", set the default extension to .jpg
        if command == "draw":
            output_extension = ".jpg"

        if command in commands_requiring_output or (command == "verify" and extra_args and "-f" in extra_args):
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

            # For "draw", skip displaying the file content as it is an image
            if command == "draw":
                messagebox.showinfo("Info", f"Graph saved as: {output_file}")
            elif output_file and os.path.exists(output_file):
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




# ////////////// Mission Done  ///////////////////////