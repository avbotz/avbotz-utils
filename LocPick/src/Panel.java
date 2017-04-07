import javax.imageio.ImageIO;
import javax.swing.*;
import java.awt.*;
import java.util.*;
import java.util.List;

public class Panel extends JPanel {

    private Image dispImage;
    public static int FRAME_WIDTH = Main.IMG_WIDTH;
    public static int FRAME_HEIGHT = Main.IMG_HEIGHT;

    public Panel() {
        this.setPreferredSize(new Dimension(FRAME_WIDTH, FRAME_HEIGHT));
    }

    public void setDispImage(Image image) {
        //System.out.println("Setting new image!");
        this.dispImage = image;
        repaint();
    }

    @Override
    public void paintComponent(Graphics g) {
        //System.out.println("Painting!");
        //System.out.println(this.dispImage.toString());
        g.drawImage(this.dispImage, 0, 0, FRAME_WIDTH, FRAME_HEIGHT, null);
    }
}
