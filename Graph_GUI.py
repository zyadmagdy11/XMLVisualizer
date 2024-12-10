import sys
import networkx as nx
import pydot
import os
from PyQt5.QtWidgets import QApplication, QMainWindow, QVBoxLayout, QWidget
from PyQt5.QtCore import Qt
from PyQt5.QtGui import QPainter, QPixmap
import matplotlib.pyplot as plt
import io

class GraphWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Social Network Graph")
        self.setGeometry(100, 100, 1200, 1000)

        self.graph = None
        self.layout = QVBoxLayout()

        # Widget to display the graph
        self.graphWidget = QWidget(self)
        self.layout.addWidget(self.graphWidget)

        # Set the central widget
        container = QWidget()
        container.setLayout(self.layout)
        self.setCentralWidget(container)

        # Initialize pixmap to None
        self.pixmap = None

        # Automatically load the graph from the 'social_network.dot' file in the same directory as the script
        script_dir = os.path.dirname(os.path.abspath(__file__))  # Get the directory of the running script
        dot_file_path = os.path.join(script_dir, 'social_network.dot')  # Construct the full path to the .dot file

        self.load_dot_file(dot_file_path)

    def load_dot_file(self, file_path):
        # Load the DOT file using pydot
        try:
            print(f"Loading DOT file: {file_path}")
            (graph,) = pydot.graph_from_dot_file(file_path)
            print("DOT file loaded successfully.")

            # Convert the pydot graph to a networkx graph
            self.graph = nx.nx_pydot.from_pydot(graph)
            print(f"Graph loaded with {len(self.graph.nodes)} nodes and {len(self.graph.edges)} edges.")

            # Trigger the re-rendering of the graph
            self.update_graph()
        except Exception as e:
            print(f"Error loading DOT file: {e}")

    def update_graph(self):
        if self.graph:
            # Adjust the spring layout parameters for more spacing
            fig, ax = plt.subplots(figsize=(12, 8))
            
            # Use spring_layout to adjust node positions and spread them apart
            pos = nx.spring_layout(self.graph, k=22, iterations=50)  # Adjust k and iterations

            # Draw the graph with adjusted spacing, applying 'connectionstyle' to spread arrows
            nx.draw(self.graph, pos, with_labels=True, node_size=4000, node_color="lightblue", font_size=12, ax=ax,
                    arrows=True, connectionstyle="arc3,rad=0.7", edgecolors='black')  # Spread arrows out

            # Save the rendered graph as a PNG in memory (instead of saving to file)
            buf = io.BytesIO()
            plt.savefig(buf, format='png')
            buf.seek(0)

            # Convert the image from BytesIO to QPixmap
            self.pixmap = QPixmap()
            self.pixmap.loadFromData(buf.read())
            print("Graph rendered and pixmap created.")
            
            # Force a repaint of the widget
            self.graphWidget.repaint()

    def paintEvent(self, event):
        if self.pixmap:
            # Paint the graph image to the window
            painter = QPainter(self)
            painter.drawPixmap(0, 0, self.pixmap)
            painter.end()
        else:
            print("No pixmap available to paint.")

def main():
    app = QApplication(sys.argv)
    window = GraphWindow()
    window.show()
    sys.exit(app.exec_())

if __name__ == "__main__":
    main()
