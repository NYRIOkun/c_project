
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
    
    
});