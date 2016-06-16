--TEST--
Testing include
--FILE--
<?php

function fn1685575344()
{
    include "015.inc";
}
fn1685575344();
--EXPECT--
Hello
