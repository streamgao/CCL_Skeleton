
var scanvas = $('#scanvas');


$(document).ready(function(){ 

	var htmlaudio = document.getElementById("gaoshanliushui");
	var scanvas = $('#scanvas');

	console.log(htmlaudio);

	htmlaudio.play();
	htmlaudio.loop = true;


	$(document).click(function(e){		
		var x= e.pageX;
		var slots = x/$(document).width();

		switch (true){
			case (slots>=0 && slots<0.1): 
				transitTime( 80, e.pageX, e.pageY );  break;
			case (slots>=0.1 && slots<0.2):
				transitTime( 40, e.pageX, e.pageY );  break;
			case (slots>=0.2 && slots<0.3):
				transitTime( 1, e.pageX, e.pageY );  break;
			case (slots>=0.3 && slots<0.4):
				transitTime( 116, e.pageX, e.pageY );  break;
				//htmlaudio.currentTime=116; break;
			case (slots>=0.4 && slots<0.5):
				transitTime( 210, e.pageX, e.pageY );  break;
				//htmlaudio.currentTime=144; break;
			case (slots>=0.5 && slots<0.6):
				transitTime( 189, e.pageX, e.pageY );  break;
				//htmlaudio.currentTime=160; break;
			case (slots>=0.6 && slots<0.7):
				transitTime( 160, e.pageX, e.pageY );  break;
				//htmlaudio.currentTime=189; break;
			case (slots>=0.7 && slots<0.8):
				transitTime( 144 , e.pageX, e.pageY );  break;
				//htmlaudio.currentTime=210; break;
			case (slots>=0.8 && slots<0.9):
				transitTime( 284, e.pageX, e.pageY );  break;
				//htmlaudio.currentTime=284; break;
			default:
				htmlaudio.currentTime=0; break;
		}
		
	});
});


function transitTime( slots, mousex, mousey ){
	$('.splash').remove();

	mousex = mousex<0? 0 : mousex-100;
	mousex = mousex>($(document).width()-200) ? ($(document).width()-200) : mousex-100;
	mousey = mousey<0? 0 : mousey-100;
	mousey = mousey>($(document).height()-200) ? ($(document).height()-200) : mousey-100;

	//change audio timing
	document.getElementById("gaoshanliushui").currentTime = slots;

	var splash =$('<div/>',{
    	'class': "splash"
	}).appendTo('.container');   // create div
	splash = splash.prevObject[0];

	splash.style.top = mousey+'px';
	splash.style.left = mousex+'px';

	console.log(splash.style);
	console.log(splash);

	var i=0;

	setInterval(function(){ 
		if(i<11){
			splash.style.background = 'url(img/splash/splash'+i+'.png) center center no-repeat';
			splash.style.backgroundSize ='100% 100%';
			i++;
		}
	}, 150);  //every 0.1 second


}




