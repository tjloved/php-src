--TEST--
Simple POST Method test
--POST--
a=Hello+World
--FILE--
<?php

function fn1648038759()
{
    echo $_POST['a'];
}
fn1648038759();
--EXPECT--
Hello World
