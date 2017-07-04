--TEST--
Simple POST Method test
--POST--
a=Hello+World
--FILE--
<?php

function fn1365751742()
{
    echo $_POST['a'];
}
fn1365751742();
--EXPECT--
Hello World
