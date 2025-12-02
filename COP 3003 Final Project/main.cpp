#include <QApplication>
#include <QPainter>
#include <QKeyEvent>
#include <QPixmap>
#include <QTimer>
#include <vector>
#include <QString>
#include <QMessageBox>


class Fishhook : public QWidget
{
protected:
    int hook_Y = 100;
    const int hookHeight = 40;
    const int hookWidth = 20;
    QTimer *timer;
    QPixmap background;

public:
    int score = 0;
    explicit Fishhook(QWidget *parent = nullptr) : QWidget(parent)
    {
        setFixedSize(600, 600);
        setFocusPolicy(Qt::StrongFocus);
        background.load("SpriteImages/beachbg.png");
    }

protected:
    void paintEvent(QPaintEvent *) override // This will display a blue background underneath the beach bg Arantxa drew, also accounting for if the image is null
    {
        QPainter painter(this);


        if (!background.isNull())
            painter.drawPixmap(rect(), background); // This accounts for if the background is a null image, will scale to the window of 600x600
        else
            painter.fillRect(rect(), QColor(0, 100, 200)); // Will draw the rest of the background as blue

        // This draws the hook itself
        painter.setBrush(Qt::gray);
        painter.drawRect(290, hook_Y, hookWidth, hookHeight);
        painter.drawLine(300, 20, 300, hook_Y);

        drawSeaLife(painter);

        // Displays the score
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 14, QFont::Bold));
        painter.drawText(10, 30, QString("Score: %1").arg(score));
    }


    void keyPressEvent(QKeyEvent *event) override //Up and Down Arrow Keys
    {
        if (event->key() == Qt::Key_Up && hook_Y > 0)
            hook_Y -= 10;
        else if (event->key() == Qt::Key_Down && hook_Y < height() - hookHeight)
            hook_Y += 10;

        update(); //Updates as user input does
    }

    virtual void drawSeaLife(QPainter &painter)
    {
    }
};


class Sealife
{
protected:
    QPixmap image;
    QPoint position;
    int speedX;
    int speedY;
    QSize scaledSize;

public:
    Sealife(const QString &imagePath, QPoint pos, QSize targetSize, int speedX = 2, int speedY = 0) //Constructor to initalize the fishes' file, position, and speed
        : position(pos), speedX(speedX), speedY(speedY)
    {
        QPixmap original(imagePath);
        image = original.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        scaledSize = image.size();
    }

    //Destructor made for the Sealife class
    virtual ~Sealife() = default;

    //Polymorphism function made that initializes the fishes' movement
    virtual void move(int width, int height)
    {
        position.setX(position.x() + speedX);
        position.setY(position.y() + speedY);

        // Loops the fish animation
        if (position.x() > width)
            position.setX(-image.width());
        if (position.x() < -image.width())
            position.setX(width);
        if (position.y() < 0 || position.y() > height - image.height())
            speedY = -speedY;
    }

    //Function that initializes where the fishes are drawn
    virtual void draw(QPainter &painter)
    {
        painter.drawPixmap(position, image);
    }

    //This connects the fishing hook to the sealife creatures
    QRect getRect() const
    {
        return QRect(position, image.size());
    }
};

//Below are the classes for the individual fishes and enemies. Each vary the speed and size
class Fish : public Sealife
{
public:
    Fish(QPoint pos) : Sealife("SpriteImages/Fish.png", pos, QSize(60, 40), -10, 0)
    {
    }
};

class Jellyfish : public Sealife
{
public:
    Jellyfish(QPoint pos) : Sealife("SpriteImages/Jellyfish.png", pos, QSize(100, 60), 6, 0)
    {
    }
};

class Sharks : public Sealife
{
public:
    Sharks(QPoint pos) : Sealife("SpriteImages/Shark.png", pos, QSize(400, 100), -5, 0)
    {
    }
};

class OscarFish : public Sealife
{
public:
    OscarFish(QPoint pos) : Sealife("SpriteImages/OscarFish.png", pos, QSize(800, 100), -1, 0)
    {
    }
};


class OceanScene : public Fishhook //This class is the entire Oceanscene, which creates the game (showing the fish and having them move, showing the fisherman, on top of screen)
{
    std::vector<Sealife *> creatures; //Vector created that moves our fish and enemies, created with starting points that move across the screen
    QPixmap fishermanImage;
    QPoint fishermanPos;
    QTimer timer;

public:
    explicit OceanScene(QWidget *parent = nullptr) : Fishhook(parent)
    {
        //Below draws the fisherman into the oceanscene, loads it on top of the screen, scales it to a certain size, then lines it up to the fishing hook
        QPixmap raw("SpriteImages/GenesisFish.png");
        fishermanImage = raw.scaled(QSize(180, 160), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        fishermanPos = QPoint(229 - fishermanImage.width() / 2, 0);
        //Below pushes back the sea creatures into the ocean scene, varying at different x and y coordinates
        creatures.push_back(new Fish(QPoint(100, 200)));
        creatures.push_back(new Fish(QPoint(500, 420)));
        creatures.push_back(new Jellyfish(QPoint(400, 170)));
        creatures.push_back(new Jellyfish(QPoint(400, 400)));
        creatures.push_back(new Sharks(QPoint(250, 300)));
        creatures.push_back(new OscarFish(QPoint(250, 300)));
        //Below shows that every 50 ms, the creatures will move forward, repaint the ocean scene, check for collisions
        connect(&timer, &QTimer::timeout, this, &OceanScene::updatePositions);
        timer.start(50);
    }

    //Destructor that will delete the sea creature's data so that the game isn't creating too much memory
    ~OceanScene() override
    {
        for (auto c: creatures)
            delete c;
    }

protected:
    void drawSeaLife(QPainter &painter) override
    {
        painter.drawPixmap(fishermanPos, fishermanImage); //This draws the fisherman as a static image
        for (auto c: creatures) //This draws the sea creatures after the base class (fishhook and background) is drawn
            c->draw(painter);
    }

private:
    void updatePositions() //This updates the positions of the sea creatures, constantly updating and checking for collisions
    {
        for (auto c: creatures)
            c->move(width(), height());

        checkCollisions();
        update();
    }

    void gameOver() //Function that will end the game once they reach 20 points
    {
        timer.stop();
        QMessageBox::information(this, "Game Over", "Congrats! You've reached the max points!");
        close(); // closes the OceanScene window
    }


    void checkCollisions() //Function made to account for when the uses connect the fishing pole with a fish or enemy
    {
        QRect hookRect(290, hook_Y, hookWidth, hookHeight);

        for (auto c: creatures)
        {
            if (c->getRect().intersects(hookRect))
            {
                if (dynamic_cast<Fish *>(c))
                {
                    score++;
                } else if (dynamic_cast<Sharks *>(c))
                {
                    score--;
                    try //This try-catch-throw will ensure that the score cannot go below 0
                    {
                        if (score < 0)
                        {
                            throw score = 0;
                        } else
                        {
                            throw score;
                        }
                    } catch (...)
                    {
                        score;
                    }
                } else if (dynamic_cast<Jellyfish *>(c))
                {
                    score--;
                    try
                    {
                        if (score < 0)
                        {
                            throw score = 0;
                        } else
                        {
                            throw score;
                        }
                    } catch (...)
                    {
                        score;
                    }
                }


                c->move(-600, -600); //This moves the creatures off the screen once it collides with a fish
                if (score >= 5 ) //One score reaches the given points, it will call the game over function
                {
                    gameOver();
                    return;
                }
            }
        }
    }
};


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    OceanScene ocean; //Creates an object of the OceanScene, with the object named ocean
    ocean.setWindowTitle("Go Fishing with First Class Solider: Genesis!"); //Shows the title screen
    ocean.show();

    return app.exec();
}