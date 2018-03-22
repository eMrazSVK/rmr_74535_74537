#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //tu je napevno nastavena ip. treba zmenit na to co ste si zadali do text boxu alebo nejaku inu pevnu. co bude spravna
    //ipaddress="192.168.0.13";
    ipaddress="192.168.0.11";
    ui->setupUi(this);
    datacounter=0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setBrush(Qt::black);
    QPen pero;
    pero.setStyle(Qt::SolidLine);
    pero.setWidth(3);
    pero.setColor(Qt::green);
    QRect rect(20,120,700,500);
    rect= ui->frame->geometry();
    rect.translate(0,15);
    painter.drawRect(rect);
    if(updateLaserPicture==1)
    {
        updateLaserPicture=0;

        painter.setPen(pero);
        //teraz tu kreslime random udaje... vykreslite to co treba... t.j. data z lidaru
        for(int k=0;k<copyOfLaserData.numberOfScans/*360*/;k++)
        {
          /*  int dist=rand()%500;
            int xp=rect.width()-(rect.width()/2+dist*2*sin((360.0-k)*3.14159/180.0))+rect.topLeft().x();
            int yp=rect.height()-(rect.height()/2+dist*2*cos((360.0-k)*3.14159/180.0))+rect.topLeft().y();*/
            int dist=copyOfLaserData.Data[k].scanDistance/10;
            int xp=rect.width()-(rect.width()/2+dist*2*sin((360.0-copyOfLaserData.Data[k].scanAngle)*3.14159/180.0))+rect.topLeft().x();
            int yp=rect.height()-(rect.height()/2+dist*2*cos((360.0-copyOfLaserData.Data[k].scanAngle)*3.14159/180.0))+rect.topLeft().y();
            if(rect.contains(xp,yp))
                painter.drawEllipse(QPoint(xp, yp),2,2);
        }
    }
}


void MainWindow::processThisRobot()
{
    //tu mozete robit s datami z robota..napriklad pozerat ci nam niekam nenarazil
     // ale nic vypoctovo narocne - to iste vlakno ktore cita data z robota
    //teraz tu len vypisujeme data z robota. vyratajte si polohu. a vypiste spravnu

    if (!init_complete) {
        x0 = 0;
        y0 = 0;
        x1 = 0;
        y1 = 0;
        fi0 = 0;
        fi1 = 0;

        previousEncoderRight = robotdata.EncoderRight;
        previousEncoderLeft = robotdata.EncoderLeft;

        init_complete = true;
    }



    diff_right = robotdata.EncoderRight - previousEncoderRight;
    diff_left  = robotdata.EncoderLeft  - previousEncoderLeft;
    //------------------------------------------------------pretecenie encoderov---------------
    if ((diff_right >= 40000) || (diff_right <= -40000)){
        if ((previousEncoderRight > 32000) && (robotdata.EncoderRight < 32000)){
            diff_right = 65535 - previousEncoderRight + robotdata.EncoderRight;
        }else{
            diff_right = 65535 - robotdata.EncoderRight + previousEncoderRight;
        }
    }
    if ((diff_left >= 40000) || (diff_left <= -40000)){
        if ((previousEncoderLeft > 32000) && (robotdata.EncoderLeft < 32000)){
            diff_left = 65535 - previousEncoderLeft + robotdata.EncoderLeft;
        }else{
            diff_left = 65535 - robotdata.EncoderLeft + previousEncoderLeft;
        }
    }
    //.........................................................................................

    fi1 = fi0 + (0.000085292090497737556558*((diff_right) - (diff_left)))/0.23;
    if ((diff_left == diff_right)) {
        x1 = x0 + (0.000085292090497737556558 * diff_left * cos(fi1));
        y1 = y0 + (0.000085292090497737556558 * diff_left * sin(fi1));
    }
    else {
        x1 = x0 + ((0.23*0.5) *((diff_right + diff_left)/(diff_right - diff_left))) * (sin(fi1) - sin(fi0));
        y1 = y0 - ((0.23*0.5) *((diff_right + diff_left)/(diff_right - diff_left))) * (cos(fi1) - cos(fi0));
    }

if (navigacia == true){
    xz1 = xz - x1;
    yz1 = yz - y1;

    d = sqrt(yz1*yz1 + xz1*xz1);  // vzdialenost od zelanej polohy
    fiz1 = atan(yz1/xz1);
    fiz = fiz1 - fi1;   // vychylenie fi od smeru k zelanej polohe

    dt=robotdata.timestamp - previous_timestamp;

    rychlost = Pp*d;

    radius = fiz*Pr;
    //integral= integral + 10/fiz*dt;
    radius = 200/fiz;// + integral*Ir;
    if (d > 0.4)
        rychlost=650;


    if (rychlost >= 650)
        rychlost = 650;

    if (radius >= 32000)
        radius=32000;
    if (radius <= -32000)
        radius=-32000;


    if (previous_speed < rychlost-5){
        previous_speed = previous_speed + 5;
        speed = previous_speed;
    }else{
        speed = rychlost;
    }


    std::vector<unsigned char> mess=robot.setArcSpeed(speed, radius );
    if (sendto(rob_s, (char*)mess.data(), sizeof(char)*mess.size(), 0, (struct sockaddr*) &rob_si_posli, rob_slen) == -1)
    {

    }


    if ((x1 < xz+0.02) && (x1 > xz-0.02)){// && (y1 > yz-0.03) && (yz < yz+0.03)){  // sem vlozit navigaciu
        std::vector<unsigned char> mess=robot.setTranslationSpeed(0);
        if (sendto(rob_s, (char*)mess.data(), sizeof(char)*mess.size(), 0, (struct sockaddr*) &rob_si_posli, rob_slen) == -1)
        {

        }
        navigacia = false;
        previous_speed = 0;
        speed=0;
    }
}




    if(datacounter%5==0)
    {
    ui->lineEdit_2->setText(QString::number(x1));
    ui->lineEdit_3->setText(QString::number(y1));
    ui->rad->setText(QString::number(radius));

    ui->lineEdit_4->setText(QString::number( (180/3.14)*fi1));

    ui->Xziadana->setText(QString::number(xz));
    ui->Yziadana->setText(QString::number(yz));
    ui->Rotodchylka->setText(QString::number(fiz));
    ui->dodchylka->setText(QString::number(d));
    }


    datacounter++;
    fi0 = fi1;
    x0=x1;
    y0=y1;
    previousEncoderRight = robotdata.EncoderRight;
    previousEncoderLeft = robotdata.EncoderLeft;
    previous_timestamp = robotdata.timestamp;

}

void MainWindow::processThisLidar(LaserMeasurement &laserData)
{
    memcpy( &copyOfLaserData,&laserData,sizeof(LaserMeasurement));
    //tu mozete robit s datami z lidaru.. napriklad najst prekazky, zapisat do mapy. naplanovat ako sa prekazke vyhnut.
     // ale nic vypoctovo narocne - to iste vlakno ktore cita data z lidaru
    updateLaserPicture=1;
    update();//tento prikaz prinuti prekreslit obrazovku.. zavola sa paintEvent funkcia



}


void MainWindow::on_pushButton_10_clicked() //LOD button
{
    xz=2;
    yz=2;
    Pp=2000;
    Pr=500;
    Ir=2500;
    integral=0;
    dt=0.1;
    previous_timestamp=robotdata.timestamp;
    previous_speed = 0;
    speed=0;

    ui->Xziadana->setText(QString::number(xz));
    ui->Yziadana->setText(QString::number(yz));
    navigacia = true;
    xz1 = xz - x1;
    yz1 = yz - y1;

    d = sqrt(yz1*yz1 + xz1*xz1);  // vzdialenost od zelanej polohy
    fiz1 = atan(yz1/xz1);
    fiz = fiz1 - fi1;   // vychylenie fi od smeru k zelanej polohe

    x0 = 0;
    y0 = 0;
    x1 = 0;
    y1 = 0;
    fi0 = 0;
    fi1 = 0;

    previousEncoderRight = robotdata.EncoderRight;
    previousEncoderLeft = robotdata.EncoderLeft;

}

void MainWindow::on_pushButton_9_clicked() //start button
{
    init_complete = false;
    //tu sa nastartuju vlakna ktore citaju data z lidaru a robota
     laserthreadID=pthread_create(&laserthreadHandle,NULL,&laserUDPVlakno,(void *)this);
     robotthreadID=pthread_create(&robotthreadHandle,NULL,&robotUDPVlakno,(void *)this);
    previous_speed = 0;
    speed = 0;
}

void MainWindow::on_pushButton_2_clicked() //forward
{
    //pohyb dopredu
    std::vector<unsigned char> mess=robot.setTranslationSpeed(250);
    //std::vector<unsigned char> mess=robot.setArcSpeed(200, 1000 );
    if (sendto(rob_s, (char*)mess.data(), sizeof(char)*mess.size(), 0, (struct sockaddr*) &rob_si_posli, rob_slen) == -1)
    {

    }
}

void MainWindow::on_pushButton_3_clicked() //back
{
    std::vector<unsigned char> mess=robot.setTranslationSpeed(-250);
    if (sendto(rob_s, (char*)mess.data(), sizeof(char)*mess.size(), 0, (struct sockaddr*) &rob_si_posli, rob_slen) == -1)
    {

    }
}

void MainWindow::on_pushButton_6_clicked() //left
{

    std::vector<unsigned char> mess=robot.setRotationSpeed(M_PI/2);
    if (sendto(rob_s, (char*)mess.data(), sizeof(char)*mess.size(), 0, (struct sockaddr*) &rob_si_posli, rob_slen) == -1)
    {

    }
}

void MainWindow::on_pushButton_5_clicked()//right
{

    std::vector<unsigned char> mess=robot.setRotationSpeed(-M_PI/2);
    if (sendto(rob_s, (char*)mess.data(), sizeof(char)*mess.size(), 0, (struct sockaddr*) &rob_si_posli, rob_slen) == -1)
    {

    }
}

void MainWindow::on_pushButton_4_clicked() //stop
{
    navigacia=false;
    std::vector<unsigned char> mess=robot.setTranslationSpeed(0);
    if (sendto(rob_s, (char*)mess.data(), sizeof(char)*mess.size(), 0, (struct sockaddr*) &rob_si_posli, rob_slen) == -1)
    {

    }
}

void MainWindow::laserprocess()
{

    int iResult = 0;



    // Initialize Winsock

    las_slen = sizeof(las_si_other);
    if ((las_s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {

    }

    int las_broadcastene=1;
    setsockopt(las_s,SOL_SOCKET,SO_BROADCAST,(char*)&las_broadcastene,sizeof(las_broadcastene));
    // zero out the structure
    memset((char *) &las_si_me, 0, sizeof(las_si_me));

    las_si_me.sin_family = AF_INET;
    las_si_me.sin_port = htons(52999);
    las_si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    las_si_posli.sin_family = AF_INET;
    las_si_posli.sin_port = htons(5299);
    las_si_posli.sin_addr.s_addr = inet_addr(ipaddress.data());//htonl(INADDR_BROADCAST);
    bind(las_s , (struct sockaddr*)&las_si_me, sizeof(las_si_me) );
    char command=0x00;
    if (sendto(las_s, &command, sizeof(command), 0, (struct sockaddr*) &las_si_posli, las_slen) == -1)
    {

    }
    LaserMeasurement measure;
    while(1)
    {
        if ((las_recv_len = recvfrom(las_s, (char*)&measure.Data, sizeof(LaserData)*1000, 0, (struct sockaddr *) &las_si_other,&las_slen)) == -1)
        {

            continue;
        }
        measure.numberOfScans=las_recv_len/sizeof(LaserData);
        //tu mame data..zavolame si funkciu

        //     memcpy(&sens,buff,sizeof(sens));


        processThisLidar(measure);




    }
}


void MainWindow::robotprocess()
{


    if ((rob_s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {

    }

    char rob_broadcastene=1;
    setsockopt(rob_s,SOL_SOCKET,SO_BROADCAST,&rob_broadcastene,sizeof(rob_broadcastene));
    // zero out the structure
    memset((char *) &rob_si_me, 0, sizeof(rob_si_me));

    rob_si_me.sin_family = AF_INET;
    rob_si_me.sin_port = htons(53000);
    rob_si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    rob_si_posli.sin_family = AF_INET;
    rob_si_posli.sin_port = htons(5300);
    rob_si_posli.sin_addr.s_addr =inet_addr(ipaddress.data());//inet_addr("10.0.0.1");// htonl(INADDR_BROADCAST);
    rob_slen = sizeof(rob_si_me);
    bind(rob_s , (struct sockaddr*)&rob_si_me, sizeof(rob_si_me) );

    std::vector<unsigned char> mess=robot.setDefaultPID();
    if (sendto(rob_s, (char*)mess.data(), sizeof(char)*mess.size(), 0, (struct sockaddr*) &rob_si_posli, rob_slen) == -1)
    {

    }
    usleep(100*1000);
    mess=robot.setSound(440,1000);
    if (sendto(rob_s, (char*)mess.data(), sizeof(char)*mess.size(), 0, (struct sockaddr*) &rob_si_posli, rob_slen) == -1)
    {

    }
   unsigned char buff[50000];
    while(1)
    {
        memset(buff,0,50000*sizeof(char));
        if ((rob_recv_len = recvfrom(rob_s, (char*)&buff, sizeof(char)*50000, 0, (struct sockaddr *) &rob_si_other, &rob_slen)) == -1)
        {

            continue;
        }
        //tu mame data..zavolame si funkciu
        //printf("encoderRight: %d, encoderLeft: %d\n", robotdata.EncoderRight, robotdata.EncoderLeft);

        //     memcpy(&sens,buff,sizeof(sens));
        struct timespec t;
  //      clock_gettime(CLOCK_REALTIME,&t);

        int returnval=robot.fillData(robotdata,(unsigned char*)buff);
        if(returnval==0)
        {
            processThisRobot();
        }


    }
}



