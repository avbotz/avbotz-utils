import javax.imageio.ImageIO;
import javax.swing.*;
import java.awt.*;
import java.util.*;
import java.util.List;

public class Panel extends JPanel {

    private Image dispImage;
    public static int FRAME_WIDTH = Main.IMG_WIDTH;
    public static int FRAME_HEIGHT = Main.IMG_HEIGHT;

	int index;
	int x=0,y=0,x2=0,y2=0;
    public Panel() {
        this.setPreferredSize(new Dimension(FRAME_WIDTH, FRAME_HEIGHT));
    }
	public void setDraw(int a, int b, int a2, int b2, int ind){
		this.x = a;
		this.y = b;
		this.x2 = a2;
		this.y2 = b2;
		repaint();	
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
		g.setColor(new Color(50,50,50,100));
		g.fillRect(x,y,x2-x,y2-y);
    }
}
