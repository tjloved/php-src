--TEST--
Bug #45986 (wrong error message for a non existent file on rename)
--CREDITS--
Sebastian Schürmann
sebs@php.net
Testfest 2009 Munich 
--FILE--
<?php

function fn355511248()
{
    rename('foo', 'bar');
}
fn355511248();
--EXPECTF--
Warning: %s in %sbug45986.php on line %d
