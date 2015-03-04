#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    n = 8; // divisions per step
    count = 0; //instructions counter

    numCoords = 0;

	ofSetVerticalSync(true);
	//ofSetFrameRate(60);

	ofBackground(40,40,40);
    
	font.loadFont("franklinGothic.otf", 12);
    smallFont.loadFont("franklinGothic.otf", 10);

    aDir = 12;
    aStp = 13;
    bDir = 9;
    bStp = 8;
    standoff = 3;

    currentDraw = false;


    MSEP = 152.0;
    AX   = 0.0;
    BX   = AX+MSEP;

    SPN = 12.5;
    /* N.B. 
     * each step is 0.225 degrees
     * there are 1600 steps to a full motor rotation
     * each motor is 100 notches from the pen at the start
     * there are 100 steps to a full notch turn
     */
    MASteps = 1250*n; // with these settings the pointer
    MBSteps = 1250*n; // will start at x=76, y=65.

	ard.connect("/dev/ttyACM0", 57600);
	
	ofAddListener(ard.EInitialized, this, &ofApp::setupArduino);
	bSetupArduino = false;
    readDatatoCoords("data/data");
}
//--------------------------------------------------------------
void ofApp::update(){
	updateArduino();
}

//--------------------------------------------------------------
void ofApp::setupArduino(const int & version) {
	
	ofRemoveListener(ard.EInitialized, this, &ofApp::setupArduino);
    
    bSetupArduino = true;

    ard.sendDigitalPinMode(aDir, ARD_OUTPUT);
    ard.sendDigitalPinMode(aStp, ARD_OUTPUT);
    ard.sendDigitalPinMode(bDir, ARD_OUTPUT);
    ard.sendDigitalPinMode(bStp, ARD_OUTPUT);
    // draw on/off servo
//    ard.sendServoAttach(standoff);

//    ard.sendDigitalPinMode(standoff, ARD_PWM);
    ard.sendDigitalPinMode(standoff, ARD_OUTPUT);
//    ofAddListener(ard.EDigitalPinChanged, this, &ofApp::digitalPinChanged);
//    ofAddListener(ard.EAnalogPinChanged, this, &ofApp::analogPinChanged);
}
//--------------------------------------------------------------
void ofApp::digitalPinChanged(const int & pinNum) {
}
//--------------------------------------------------------------
void ofApp::analogPinChanged(const int & pinNum) {
}

//--------------------------------------------------------------
float ofApp::getCurrentX(){
    return ((pow((MASteps)/(SPN*n),2)-pow((MBSteps)/(SPN*n),2)-pow(AX,2)+pow(BX,2))/(2*(BX-AX)));
}
//--------------------------------------------------------------
float ofApp::getCurrentY(){
    return sqrt(pow(MASteps/(SPN*n),2)-pow(getCurrentX()-AX,2));
}
//--------------------------------------------------------------
void ofApp::movePointerTo(float newX, float newY){

    int newAsteps = floor(sqrt(pow(newY,2)+pow(newX-AX,2))*(SPN*n));
    int newBsteps = floor(sqrt(pow(newY,2)+pow(newX-BX,2))*(SPN*n));
    
    int changeA = newAsteps - MASteps;
    int changeB = newBsteps - MBSteps;

    if(changeA > 0){
        ard.sendDigital(aDir, ARD_HIGH);  // CW
    }else if(changeA < 0){
        ard.sendDigital(aDir, ARD_LOW); // CCW
    }

    for(int i=0; i<abs(changeA); i++){
        ard.sendDigital(aStp,ARD_LOW);
        ard.sendDigital(aStp,ARD_HIGH);
        ofSleepMillis(2);
    }

    if(changeB > 0){
        ard.sendDigital(bDir, ARD_LOW);  // CCW
    }else if(changeB < 0){
        ard.sendDigital(bDir, ARD_HIGH); // CW
    }
    for(int i=0; i<abs(changeB); i++){
        ard.sendDigital(bStp,ARD_LOW);
        ard.sendDigital(bStp,ARD_HIGH);
        ofSleepMillis(2);
    }

    MASteps = newAsteps;
    MBSteps = newBsteps;

}

//--------------------------------------------------------------
void ofApp::straightLineTo(float newX, float newY){
    ofVec2f start(getCurrentX(),getCurrentY());
    ofVec2f pos(getCurrentX(),getCurrentY());
    ofVec2f end(newX,newY);
    float dist = start.distance(end);
    updateDistGraph(floor(dist));
    cout << " | d:" << ofToString(dist) << endl;
    float stepSize = 0.1/dist;
    for(float i=stepSize; i<=1; i+=stepSize){
        if(i>1) i=1;
        pos.interpolate(end, i);
        movePointerTo(pos.x,pos.y);
        pos.set(start.x,start.y);
    }
}

//--------------------------------------------------------------
void ofApp::updateDistGraph(int n){
    for(int i=10;i>0;i--){
        distGraph[i] = distGraph[i-1];
    }
    distGraph[0] = n;
}
        
//--------------------------------------------------------------
void ofApp::drawing(bool d){
    if(d){
        if(currentDraw != d){
            ard.sendDigital(standoff,ARD_HIGH);
            ofSleepMillis(500);
        }
        currentDraw = d;
    }else if(!d){
        if(currentDraw != d){
            ard.sendDigital(standoff,ARD_LOW);
            ofSleepMillis(500);
        }
        currentDraw = d;
    }
}
//--------------------------------------------------------------
void ofApp::readDatatoCoords(string filepath){
    ifstream file("data/data");
    while (file)
    {
        string line;
        getline(file,line);
        istringstream row(line);
        int j=0;
        while (row)
        {
            switch(j){
                case 0: row >> coord[numCoords][0]; j++; break;
                case 1: row >> coord[numCoords][1]; j++; break;
                case 2: row >> coord[numCoords][2]; j++; break;
                default: break;
            }
            if(j>2)break;
        }
        numCoords++;
    }
}


//--------------------------------------------------------------
void ofApp::updateArduino(){

	ard.update();
	
	if (bSetupArduino) {

        // DRAWING INSTRUCTIONS
        float nx;
        float ny;


        if(count >= numCoords-1)ofExit();
        nx = coord[count][0];
        ny = coord[count][1];
        drawing((bool)coord[count][2]==0);
        straightLineTo(nx,ny);

        // log
        cout << ofToString(count) <<": "<< "a:" << ofToString(MASteps,5,'0') <<" b:" << ofToString(MBSteps,5,'0') 
        << " | x:" << ofToString(getCurrentX()) << " y:" << ofToString(getCurrentY()) 
        << " >> " << "x:" << ofToString(nx) << " y:" << ofToString(ny);

        count++;
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    int b = 50; // buffer
    ofFill();
    ofSetColor(20, 20, 20);
    ofRect(b*2 + MSEP*2, 0,350, ofGetWindowHeight());
    
    ofNoFill();
    ofSetColor(200, 200, 200);
    ofRect(b,b,MSEP*2,400);
    ofEllipse(b+getCurrentX()*2,b+getCurrentY()*2,10,10);
    
    
	if (!bSetupArduino){
        ofSetColor(40, 40, 40);
	} else {
        ofSetColor(164, 164, 255);
    }
    //Panel
    font.drawString("Distance",b*3 + MSEP*2 , b);
    ofBeginShape();
    for(int i=0;i<=10;i++){
        ofVertex(b*3 +MSEP*2 +i*25,b*2 -ofMap(distGraph[i],0,maxValueIn(distGraph),0,30));
    }
    ofEndShape();
    font.drawString("Direction",b*3 + MSEP*2 , b*3);
    smallFont.drawString("Absolute",b*3 + MSEP*2 , b*3+15);
    smallFont.drawString("Relative",125+ b*3 + MSEP*2 , b*3+15);
    ofEllipse(62.5+b*3 +MSEP*2, b*4 +15, 125,125);
    ofEllipse(187.5+b*3 +MSEP*2, b*4 +15, 125,125);
   
    //graph    
    ofSetColor(164, 164, 255);
    smallFont.drawString(ofToString(MASteps,5,'0'), b, b-2);
    smallFont.drawString(ofToString(MBSteps,5,'0'), 9+MSEP*2, b-2);
    smallFont.drawString("("+ofToString(getCurrentX(),1)+","+
            ofToString(getCurrentY(),1)+")", b+5+getCurrentX()*2,b+getCurrentY()*2);
    ofSetColor(255, 255, 255);

	
}

//--------------------------------------------------------------
float ofApp::maxValueIn(float array[]){
    float x = array[0];
    for(int i=0;i< sizeof array;i++){
        if(array[i]>x) x = array[i];
    }
    return x;
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
