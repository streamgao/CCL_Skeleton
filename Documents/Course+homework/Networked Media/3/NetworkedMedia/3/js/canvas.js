var container = document.getElementById("container");

window.addEventListener('load',loaded);
document.onkeypress=onKeyPress;

function loaded(){
    var width= document.getElementById("container").offsetWidth/35;   //30= width/bacgroundwidth   
	var height = document.getElementById("container").offsetHeight/34;  //18= height/ bacgroundheight 
	console.log(height+ " "+ width);
	var hackthon="~1234567890-=QWERTYUIOP[]ASDFGHJKL;'ZXCVBNM,./";
	var pickedNum;
	var picked;

	var basicBox = new Array();
	for (var j = 0; j < height;j++) {
		//basicBox[j]= new Array(width);
		for (var i = 0; i <width-1; i++) {
			var createDiv=document.createElement("DIV");
			createDiv.setAttribute("class","basic"); 
			pickedNum= Math.floor(Math.random()*46);
			picked=hackthon[pickedNum];
			createDiv.innerHTML = picked;
			container.appendChild(createDiv);
		}
	}
 	H(2);
	A(6);
	C(10);
	K(13);
	A(17);
	T(21);
	H(25);
	O(29);
	N(33);
}

function onKeyPress(){

	alert("adv ");
}



function drawOneCharacter(left, top){
	var createH = document.createElement("DIV");
	//createH.setAttribute("class","basic imgH" );
	createH.setAttribute("class","imgH");
	createH.style.left="35"*left;
	createH.style.top="34"*top;
	var hackthon="HACKATHON";
	var pickedNum= Math.floor(Math.random()*8);
	var picked=hackthon[pickedNum];
	createH.innerHTML = picked;
	container.appendChild(createH);
	console.log(picked);
}

function H(left){
	I(left);
	I(left+2);
	drawOneCharacter(left+1,7);
}

function A(left){
	H(left);
	drawOneCharacter(left+1,5);
}

function C(left){
	I(left);
	drawOneCharacter(left+1,5);
	drawOneCharacter(left+1,9);
}

function I(left){
	for (var i = 5; i < 10; i++) {           //h
		var createH = document.createElement("DIV");
		drawOneCharacter(left, i);
	}
}

function K(left){
	I(left);
	drawOneCharacter(left+2,5);
	drawOneCharacter(left+2,9);
	drawOneCharacter(left+1,6);
	drawOneCharacter(left+1,8);
}
function T(left){
	I(left+1);
	drawOneCharacter(left,5);
	drawOneCharacter(left+2,5);
}
function O(left){
	C(left);
	I(left+2);
}
function N(left){
	I(left);
	I(left+3);
	drawOneCharacter(left+1,6);
	drawOneCharacter(left+2,7);
}

