--TEST--
Trying declare interface with repeated name of inherited method
--FILE--
<?php

interface a
{
    function b();
}
interface b
{
    function b();
}
interface c extends a, b
{
}
function fn209601664()
{
    echo "done!\n";
}
fn209601664();
--EXPECTF--
done!
