$(function(){
   $(".tag").click(function(){
      $(".tag").removeClass("active");
      $(this).addClass("active");
   });
   
   $(".bar-ope").click(function(){
      $(".com").css('width','20%');
   });
   $(".bar-re").click(function(){
      $(".com").css('width','30%');
   });
   
});