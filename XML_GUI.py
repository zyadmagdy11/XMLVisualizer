import tkinter as tk
from tkinter import filedialog, messagebox, ttk
from tkinter.scrolledtext import ScrolledText
import heapq
import os
import json
import xml.etree.ElementTree as ET


class HuffmanNode:
    """Node for Huffman tree"""
    def __init__(self, char, freq):
        self.char = char
        self.freq = freq
        self.left = None
        self.right = None

    def __lt__(self, other):
        return self.freq < other.freq


def build_huffman_tree(freq_map):
    """Build the Huffman tree from frequency map"""
    heap = [HuffmanNode(char, freq) for char, freq in freq_map.items()]
    heapq.heapify(heap)

    while len(heap) > 1:
        left = heapq.heappop(heap)
        right = heapq.heappop(heap)
        merged = HuffmanNode(None, left.freq + right.freq)
        merged.left = left
        merged.right = right
        heapq.heappush(heap, merged)

    return heap[0]


def generate_codes(node, current_code="", code_map=None):
    """Generate Huffman codes for each character."""
    if code_map is None:
        code_map = {}

    if node:
        if node.char is not None:
            code_map[node.char] = current_code
        generate_codes(node.left, current_code + "0", code_map)
        generate_codes(node.right, current_code + "1", code_map)

    return code_map


def compress(input_file, output_file):
    """Compress the input XML file."""
    with open(input_file, "r") as file:
        data = file.read()

    # Build frequency map
    freq_map = {}
    for char in data:
        freq_map[char] = freq_map.get(char, 0) + 1

    # Build Huffman tree and generate codes
    root = build_huffman_tree(freq_map)
    code_map = generate_codes(root)

    # Encode the data
    encoded_data = "".join(code_map[char] for char in data)

    # Write to output file
    with open(output_file, "wb") as file:
        # Write the frequency map
        file.write(len(freq_map).to_bytes(4, "big"))
        for char, freq in freq_map.items():
            file.write(char.encode("utf-8"))
            file.write(freq.to_bytes(4, "big"))

        # Write the encoded data
        padding = 8 - (len(encoded_data) % 8)
        encoded_data += "0" * padding
        file.write(padding.to_bytes(1, "big"))

        for i in range(0, len(encoded_data), 8):
            byte = int(encoded_data[i:i + 8], 2)
            file.write(byte.to_bytes(1, "big"))


def decompress(input_file, output_file):
    """Decompress the binary file into XML."""
    with open(input_file, "rb") as file:
        # Read frequency map
        num_symbols = int.from_bytes(file.read(4), "big")
        freq_map = {}
        for _ in range(num_symbols):
            char = file.read(1).decode("utf-8")
            freq = int.from_bytes(file.read(4), "big")
            freq_map[char] = freq

        # Reconstruct Huffman tree
        root = build_huffman_tree(freq_map)

        # Read the encoded data
        padding = int.from_bytes(file.read(1), "big")
        encoded_data = ""
        byte = file.read(1)
        while byte:
            encoded_data += f"{int.from_bytes(byte, 'big'):08b}"
            byte = file.read(1)

        # Remove padding
        encoded_data = encoded_data[:-padding]

        # Decode the data
        decoded_data = ""
        current = root
        for bit in encoded_data:
            current = current.right if bit == "1" else current.left
            if current.char is not None:
                decoded_data += current.char
                current = root

    with open(output_file, "w") as file:
        file.write(decoded_data)

#######################################################################################
class XMLViewerApp:
    def __init__(self, root):
        self.root = root
        self.root.title("XML Viewer")
        self.root.geometry("900x800")
        self.root.configure(bg="#f5f5f5")

        # Title
        self.title_label = tk.Label(
            self.root,
            text="XML Viewer",
            font=("Arial", 16, "bold"),
            bg="#f5f5f5",
            fg="#333",
        )
        self.title_label.pack(pady=10)

        # Buttons
        self.buttons_frame = tk.Frame(self.root, bg="#f5f5f5")
        self.buttons_frame.pack(pady=10)

        buttons = [
            ("Load XML File", self.load_xml_file, "#2F4F4F"), 
            ("Check Consistency", self.check_consistency, "#4682B4"), 
            ("Detect Errors", self.detect_errors, "#6A5ACD"),  
            ("Correct XML", self.correct_xml, "#8A2BE2"),  
            ("Format XML", self.format_xml, "#00CED1"), 
            ("Minify XML", self.minify_xml, "#20B2AA"),  
            ("Convert to JSON", self.convert_to_json, "#32CD32"),  
            ("Compress XML", self.compress_xml, "#3CB371"), 
            ("Decompress XML", self.decompress_xml, "#FFD700"),  
        ]

        for i, (text, command, color) in enumerate(buttons):
            tk.Button(
                self.buttons_frame,
                text=text,
                command=command,
                font=("Arial", 12),
                bg=color,
                fg="#fff",
                padx=10,
                pady=5,
            ).grid(row=0, column=i, padx=5)

        # JSON Output Textbox
        self.json_output = ScrolledText(self.root, wrap=tk.WORD, height=15, font=("Arial", 10))
        self.json_output.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

        # Result Label
        self.result_label = tk.Label(self.root, text="", font=("Arial", 14, "italic"), bg="#f5f5f5", fg="#333")
        self.result_label.pack(pady=10)

        self.file_path = None

#############################################################################################
    def load_xml_file(self):
        # Open a file dialog to select an XML file
        self.file_path = filedialog.askopenfilename(filetypes=[("XML Files", "*.xml")])
        
        if self.file_path:
            try:
                # Read the content of the selected file
                with open(self.file_path, 'r') as file:
                    self.xml_content = file.read()
                
                # Display the number of characters in the file
                num_characters = len(self.xml_content)
                self.result_label.config(
                    text=f"File Loaded Successfully!\nNumber of Characters: {num_characters}",
                    fg="green"
                )
                
                # Display the XML content in the output box
                self.json_output.delete(1.0, tk.END)
                self.json_output.insert(tk.END, self.xml_content)
            
            except Exception as e:
                # Handle any errors that occur during file reading
                messagebox.showerror("Error", f"Failed to load XML file: {e}")
        else:
            # Handle case when no file is selected
            self.result_label.config(text="No file selected.", fg="red")

    def display_xml_in_treeview(self, xml_content):
        """Display the XML content in a Treeview widget."""
        self.treeview.delete(*self.treeview.get_children())  # Clear existing entries
        
        def parse_xml_to_tree(xml, parent_id=""):
            i = 0
            while i < len(xml):
                if xml[i] == '<':
                    is_closing = xml[i + 1] == '/'
                    tag_start = i + (2 if is_closing else 1)
                    tag_end = xml.find('>', tag_start)
                    if tag_end == -1:  # Malformed tag
                        break
                    tag_content = xml[tag_start:tag_end]
                    if is_closing:  # Closing tag
                        return tag_end + 1  # Return after closing tag
                    else:  # Opening tag
                        # Extract tag name and attributes
                        space_idx = tag_content.find(' ')
                        if space_idx != -1:
                            tag_name = tag_content[:space_idx]
                            attributes = tag_content[space_idx + 1:].strip()
                        else:
                            tag_name = tag_content
                            attributes = ""

                        # Insert into Treeview
                        node_id = self.treeview.insert(
                            parent_id, 'end', text=tag_name, values=(attributes or ""))
                        
                        # Recursively process child nodes
                        i = parse_xml_to_tree(xml[tag_end + 1:], parent_id=node_id)
                        continue
                i += 1  # Skip other characters
            return len(xml)

        # Parse the root node
        try:
            parse_xml_to_tree(xml_content)
        except Exception as e:
            messagebox.showerror("Error", f"Failed to display XML: {e}")

    def insert_node_in_treeview(self, node, parent=''):
        # Recursively add XML nodes to Treeview
        node_id = self.treeview.insert(parent, 'end', text=node.tag, values=(node.text.strip() if node.text else ""))
        for child in node:
            self.insert_node_in_treeview(child, node_id)

#############################################################################################
    def check_consistency(self):
        if not self.file_path:
            messagebox.showwarning("Warning", "Please load an XML file first.")
            return
        try:
            with open(self.file_path, 'r') as file:
                xml_content = file.read()

            is_consistent = self._check_xml_consistency(xml_content)
            character_count = len(xml_content)
            result = f"Consistency: {'Consistent' if is_consistent else 'Inconsistent'}\nCharacters: {character_count}"
            self.result_label.config(text=result, fg="green" if is_consistent else "red")
        except Exception as e:
            messagebox.showerror("Error", f"An error occurred: {e}")

    def _check_xml_consistency(self, xml_content):
        stack = []
        i = 0
        while i < len(xml_content):
            if xml_content[i] == '<':
                closing = xml_content[i + 1] == '/'
                tag_start = i + (2 if closing else 1)
                tag_end = xml_content.find('>', tag_start)
                if tag_end == -1:
                    # Malformed tag (missing `>`)
                    return False
                tag = xml_content[tag_start:tag_end]
                if closing:
                    if not stack or stack[-1] != tag:
                        # Closing tag doesn't match the last opened tag
                        return False
                    stack.pop()
                else:
                    stack.append(tag)
                i = tag_end
            else:
                i += 1
        # If the stack isn't empty, there are unclosed tags
        return not stack

    def detect_errors(self):
        if not self.file_path:
            messagebox.showwarning("Warning", "Please load an XML file first.")
            return
        try:
            with open(self.file_path, 'r') as file:
                xml_content = file.read()

            mismatched_positions = self._find_mismatched_tags(xml_content)
            if mismatched_positions:
                self.result_label.config(text=f"Errors at: {', '.join(map(str, mismatched_positions))}", fg="orange")
            else:
                self.result_label.config(text="No errors detected.", fg="green")
        except Exception as e:
            messagebox.showerror("Error", f"An error occurred: {e}")

    def _find_mismatched_tags(self, xml_content):
        # Detect mismatched tags
        stack = []
        mismatches = []
        for i in range(len(xml_content)):
            if xml_content[i] == '<':
                closing = xml_content[i + 1] == '/'
                tag_start = i + (2 if closing else 1)
                tag_end = xml_content.find('>', tag_start)
                tag = xml_content[tag_start:tag_end]
                if closing:
                    if not stack or stack[-1] != tag:
                        mismatches.append(i)
                    else:
                        stack.pop()
                else:
                    stack.append(tag)
        return mismatches

    def correct_xml(self):
        if not self.file_path:
            messagebox.showwarning("Warning", "Please load an XML file first.")
            return
        try:
            with open(self.file_path, 'r') as file:
                xml_content = file.read()

            corrected_xml = self._correct_mismatched_tags(xml_content)
            save_path = filedialog.asksaveasfilename(defaultextension=".xml", filetypes=[("XML Files", "*.xml")])
            if save_path:
                with open(save_path, 'w') as file:
                    file.write(corrected_xml)
                messagebox.showinfo("Success", "XML corrected and saved.")
            else:
                messagebox.showinfo("Cancelled", "Save operation cancelled.")
        except Exception as e:
            messagebox.showerror("Error", f"An error occurred: {e}")

    def _correct_mismatched_tags(self, xml_content):
        mismatched_positions = self._find_mismatched_tags(xml_content)
        tag_stack = []  # To track opening tags
        offset = 0      # To adjust for string modifications

        for pos in mismatched_positions:
            pos += offset
            if xml_content[pos + 1] == '/':  # Closing tag
                close_pos = xml_content.find('>', pos)
                xml_content = xml_content[:pos] + xml_content[close_pos + 1:]
                offset -= (close_pos - pos + 1)
            else:  # Opening tag
                close_pos = xml_content.find('>', pos)
                tag_name = xml_content[pos + 1:close_pos]
                closing_tag = f"</{tag_name}>"
                insert_pos = xml_content.find('<', close_pos + 1)
                if insert_pos == -1:  # No subsequent tags, append at the end
                    insert_pos = len(xml_content)
                xml_content = xml_content[:insert_pos] + closing_tag + xml_content[insert_pos:]
                offset += len(closing_tag)

        # Handle remaining unmatched opening tags
        while tag_stack:
            tag_name = tag_stack.pop()
            closing_tag = f"</{tag_name}>"
            xml_content += closing_tag

        return xml_content

#############################################################################################
    def convert_to_json(self):
        if not self.file_path:
            messagebox.showwarning("Warning", "Please load an XML file first.")
            return
        try:
            with open(self.file_path, 'r') as file:
                xml_content = file.read()
            json_content = self._convert_xml_to_json(xml_content)
            self.json_output.delete(1.0, tk.END)
            self.json_output.insert(tk.END, json_content)
        except Exception as e:
            messagebox.showerror("Error", f"An error occurred: {e}")

    def _convert_xml_to_json(self, xml_content):
        # Convert XML to JSON
        root = ET.fromstring(xml_content)
        def element_to_dict(element):
            return {
                "tag": element.tag,
                "text": element.text.strip() if element.text else "",
                "children": [element_to_dict(child) for child in element]
            }
        return json.dumps(element_to_dict(root), indent=2)

#############################################################################################
    def format_xml(self):
        """Format XML content and display it in the GUI."""
        if not self.file_path:
            messagebox.showwarning("Warning", "Please load an XML file first.")
            return

        try:
            formatted_xml = self._formatting_function(self.xml_content)
            self.json_output.delete(1.0, tk.END)
            self.json_output.insert(tk.END, formatted_xml)
            self.result_label.config(text="XML formatted successfully.", fg="green")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to format XML: {e}")

    def _formatting_function(self, xml_content):
        """Formatting function for XML."""
        output = []
        indentation_level = 0
        current_tag = ""

        for i, char in enumerate(xml_content):
            if char == "<":
                if current_tag.strip():
                    output.append(" " * (indentation_level * 2) + current_tag.strip())
                current_tag = char
            elif char == ">":
                current_tag += char
                if current_tag.startswith("</"):
                    indentation_level -= 1
                output.append(" " * (indentation_level * 2) + current_tag.strip())
                if not current_tag.startswith("</") and not current_tag.endswith("/>"):
                    indentation_level += 1
                current_tag = ""
            else:
                current_tag += char

        if current_tag.strip():
            output.append(" " * (indentation_level * 2) + current_tag.strip())

        return "\n".join(output)
    
#############################################################################################
    def minify_xml(self):
        """Minify XML content and display it in the GUI."""
        if not self.file_path:
            messagebox.showwarning("Warning", "Please load an XML file first.")
            return

        try:
            minified_xml = self._minifying_function(self.xml_content)
            self.json_output.delete(1.0, tk.END)
            self.json_output.insert(tk.END, minified_xml)
            self.result_label.config(text="XML minified successfully.", fg="green")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to minify XML: {e}")

    def _minifying_function(self, xml_content):
        """Minifying function for XML."""
        return "".join(line.strip() for line in xml_content.splitlines() if line.strip())
    
#############################################################################################
    def compress_xml(self):
        if not self.file_path:
            messagebox.showwarning("Warning", "Please load an XML file first.")
            return
        compressed_file = filedialog.asksaveasfilename(defaultextension=".bin", filetypes=[("Binary Files", "*.bin")])
        if not compressed_file:
            return
        try:
            compress(self.file_path, compressed_file)
            messagebox.showinfo("Success", f"File compressed and saved to {compressed_file}")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to compress XML file: {e}")

    def decompress_xml(self):
        compressed_file = filedialog.askopenfilename(filetypes=[("Binary Files", "*.bin")])
        if not compressed_file:
            return
        decompressed_file = filedialog.asksaveasfilename(defaultextension=".xml", filetypes=[("XML Files", "*.xml")])
        if not decompressed_file:
            return
        try:
            decompress(compressed_file, decompressed_file)
            messagebox.showinfo("Success", f"File decompressed and saved to {decompressed_file}")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to decompress XML file: {e}")


if __name__ == "__main__":
    root = tk.Tk()
    app = XMLViewerApp(root)
    root.mainloop()