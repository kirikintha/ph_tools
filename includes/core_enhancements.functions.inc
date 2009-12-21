<?php
/**
* Generic PHP public functions
* Helper public functions
* TODO: Figure out how to make autocmplete look on the page first, so no slow json
*/

//Handles ctype and cdigit problems for hosts with PHP install problems
if(!function_exists('ctype_alpha')) {
 
 //print 'ctype alpha does not exist';
 function ctype_alpha($text) {
   return preg_match("/[A-Za-z]/",$text);
 }
 
}

if (!function_exists('ctype_digit')) {
 
 function ctype_digit($var) {
   return ((int) $var == $var);
 }
 
}