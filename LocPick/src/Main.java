import javax.imageio.ImageIO;
import javax.swing.*;
import java.awt.*;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.image.BufferedImage;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.net.URL;
import java.util.*;
import java.util.List;

public class Main {

    private JFrame dispFrame;
    private Panel dispPanel;
    private File imagesDir;
    private File outputFile;
    private String name;
    private int idx;

    /*
    IMPORTANT: This doesn't adapt if the images are of different sizes, although I would assume they are.
     */
    public static int IMG_WIDTH;
    public static int IMG_HEIGHT;

    public static void main(String[] args) {
        new Main().init();
    }

    private void init() {
        setupImages();
        setupDisp();
    }

    private void setupImages() {

        System.out.println("Setting up images!");

        // Setup directories
        Scanner scanner = new Scanner(System.in);
        System.out.print("Directory w/ Images: ");
        imagesDir = new File(scanner.next());
        System.out.print("Output file name: ");
        outputFile = new File(scanner.next());
        try {
            outputFile.createNewFile();
        } catch (Exception e) {
            e.printStackTrace();
        }

        // Setup width and height of panel
        try {
            File file = imagesDir.listFiles()[0];
            BufferedImage image = ImageIO.read(file);
            name = file.getName();
            IMG_HEIGHT = image.getHeight();
            IMG_WIDTH = image.getWidth();
            System.out.println("Width is " + IMG_WIDTH + " px, height is " + IMG_HEIGHT + " px. File name is " + name + " .");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void setupDisp() {

        System.out.println("Setting up display!");

        // Setup panel
        idx = 0;
        dispPanel = new Panel();
        dispPanel.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent mouseEvent) {
                super.mouseClicked(mouseEvent);

                // Get click location
                int x = mouseEvent.getX();
                int y = mouseEvent.getY();

                // Manage mouse click
                manageMouseClick(x, y);

                // Display next image
                idx += 1;
                dispImage();
            }
        });
        dispImage();

        // Setup frame
        dispFrame = new JFrame();
        dispFrame.setPreferredSize(new Dimension(Panel.FRAME_WIDTH, Panel.FRAME_HEIGHT));
        dispFrame.add(dispPanel);
        dispFrame.pack();
        dispFrame.setVisible(true);
        dispFrame.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);;
    }

    private void manageMouseClick(int x, int y) {

        // Write coordinates to a file
        System.out.println("@X: " + x + " @Y: " + y + ", wrote to file!");
        try {
            FileWriter fileWriter = new FileWriter(outputFile, true);
            BufferedWriter bufferedWriter = new BufferedWriter(fileWriter);
            bufferedWriter.append(x + " " + y + " " + name);
            bufferedWriter.newLine();
            bufferedWriter.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void dispImage() {

        // Get correct image
        try {
            File file = imagesDir.listFiles()[idx];
            Image image = ImageIO.read(file);
            name = file.getName();
            dispPanel.setDispImage(image);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
