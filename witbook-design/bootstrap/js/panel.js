
$(function(){
    $(".main_showcase_img > img").hover(
        function(){
           $(this). css("opacity","0.5");
        },
        function(){
           $(this). css("opacity","1"); 
        });
    
    $(".slidetext").hover(
        function(){
           $(this).addClass("slidetoright");
        },
        function(){
           $(this).removeClass("slidetoright");
        });
    
        $(".header_menu > ul > li,.header_logo").hover(
        function(){
           $(this). css("opacity","0.5").addClass('hover_to_big');
        },
        function(){
           $(this). css("opacity","1").removeClass('hover_to_big'); 
        });
    
    /*  code from http://weboook.blog22.fc2.com/ */
    $('.box,img,.slidetext,.focus_text').css("opacity","0");
    $(window).scroll(function (){
    $('.box,img,.slidetext,.focus_text').each(function(){
      var imgPos = $(this).offset().top;    
      var scroll = $(window).scrollTop();
      var windowHeight = $(window).height();
      if (scroll > imgPos - windowHeight + windowHeight/5){
        $(this).css("opacity","1");
      }
    });
  });
    
    $(window).resize(function(){
    var winWid = $(window).width();
    var maxWid = 750;
    var OBJ = $('.object > h2 > i')
    if (winWid <= maxWid) {
        $('header').css("padding-top",'5px');
        OBJ.removeClass('fa-hand-o-left,fa-hand-o-right');
        OBJ.addClass('fa-hand-o-down');
    } else {
        $('header').css("padding-top",'10px');
        OBJ.removeClass('fa-hand-o-down');
        $('.main_focusR > .object >h2 > i').addClass('fa-hand-o-left');
        $('.main_focusL > .object >h2 > i').addClass('fa-hand-o-right');
    }
});
    
});