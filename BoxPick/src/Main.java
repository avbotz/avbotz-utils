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

	int dragindex=0;
	int[] x,y,x2,y2;

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
		x = new int[]{-1,-1,-1};
		y = new int[]{-1,-1,-1};
		x2 = new int[]{-1,-1,-1};
		y2 = new int[]{-1,-1,-1};

        System.out.println("Setting up display!");

        // Setup panel
        idx = 0;
        dispPanel = new Panel();
        dispPanel.addMouseListener(new MouseAdapter() {
            @Override
            public void mousePressed(MouseEvent mouseEvent) {
                super.mousePressed(mouseEvent);

                // Get click location
                x[dragindex] = mouseEvent.getX();
                y[dragindex] = mouseEvent.getY();
            }
			@Override
			public void mouseReleased(MouseEvent mouseEvent){
				x2[dragindex] = mouseEvent.getX();
				y2[dragindex] = mouseEvent.getY();
				manageMouseClick();
                // Display next image
				dragindex++;
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
	private boolean allPos(int[] arr){
		for(int j:arr){
			if(j==-1)
				return false;
		}
		return true;
	}
    private void manageMouseClick() {
		//check if three boxes have been made
		if(allPos(x) && allPos(y) && allPos(x2) && allPos(y2)){
			// Write coordinates to a file
			for(int l = 0; l < x.length; l++){
				System.out.print("@X"+l+": " + x[l] + " @Y"+l+": " + y[l] + " @W"+l+": " + (x2[l]-x[l])+ " @H"+l+": " + (y2[l]-y[l]) + " ");
			}
			System.out.println();
			try {
				FileWriter fileWriter = new FileWriter(outputFile, true);
				BufferedWriter bufferedWriter = new BufferedWriter(fileWriter);
				for(int l = 0; l < x.length; l++){
					bufferedWriter.append(x[l] + " " + y[l] + " " + (x2[l]-x[l])+ " " + (y2[l]-y[l]) + " ");
				}
				bufferedWriter.append(name);
				bufferedWriter.newLine();
				bufferedWriter.close();

				//reset all variables
				x = new int[]{-1,-1,-1};
				y = new int[]{-1,-1,-1};
				x2 = new int[]{-1,-1,-1};
				y2 = new int[]{-1,-1,-1};
				dragindex=0;

				//change image
                idx += 1;
                dispImage();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
    }

    private void dispImage() {

        // Get correct image
		if(imagesDir.listFiles().length<=idx){
			System.out.println("imgs over");
			System.exit(0);
		}
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
