#include "rtpserver.h"

rtpServer::rtpServer(QObject *parent) :
    rtpClass(parent)
{
    sess=new RTPSession;

}

int rtpServer::prosess_pack(int timestamp,int len,char*buf,int index)
{
    static int tmp=0;
       static int cnt=0;
    static int length;
    FILE *file;

     QString img_name;
     img_name=QString("%1.jpg").arg(index+1);


    picData=(struct pic_data *)buf;
       qDebug("offset=%d,size=%d",picData->offset,picData->length);
      memcpy(&buffer[picData->offset]+819200*index,picData->buf,1024);
       if(picData->length-picData->offset<1024)
       {
         //ֻ�е�һ֡���ݽ����������ʱ��д����Ļ


                if((file = fopen(img_name.toUtf8().data(), "wb")) < 0)
                {
                        qDebug("Unable to create y frame recording file\n");
                         exit(-1);
                }

                qDebug("receive %d,lentgth=%d,index=%d\n",picData->offset,picData->length,index);
                //printf("buffer write %d bytes into file.\n",length);
                fwrite(buffer+index*819200, picData->length, 1, file);
                fclose(file);
                    //exit(0);
                QImage img;
                if(img.load(img_name))
                 {
                    emit display(QImageToIplImage(&img),1);
                }
                //draw(img_name,index+1);

       }
       return 0;

}
void rtpServer::run()
{
    unsigned int list[10],i=0,index,SSRC;

    do {
    // ����RTP����
        sess->Poll();
        sess->BeginDataAccess();
    // ����RTP����Դ
            if (sess->GotoFirstSourceWithData())
            {


                 do {
                        RTPPacket* packet;
                    // ��ȡRTP���ݱ�
                    RTPSourceData *ssrc=sess->GetCurrentSourceInfo();
                    SSRC=ssrc->GetSSRC();



                        if((packet = sess->GetNextPacket()) != NULL) {
                        qDebug()<<packet->GetPayloadType();
                        prosess_pack(packet->GetTimestamp(),packet->GetPayloadLength(),(char *)packet->GetPayloadData(),i);


                            sess->DeletePacket(packet);
                        // ɾ��RTP���ݱ�

                    }
                } while (sess->GotoNextSourceWithData());


         }


      sess->EndDataAccess();


  } while(!stopped);
    qDebug()<<"recv thread is exit";
  return;
}
rtpServer::~rtpServer()
{
    sess->Destroy();

}
void rtpServer::rtp_recv_init(int port)
{

    int status;
    RTPUDPv4TransmissionParams transparams;
    RTPSessionParams sessparams;
    sessparams.SetOwnTimestampUnit(1.0/10.0);
    sessparams.SetAcceptOwnPackets(true);

    transparams.SetPortbase(port);

    status = sess->Create(sessparams,&transparams);
    checkerror(status);





}


