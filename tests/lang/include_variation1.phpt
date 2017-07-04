--TEST--
include() a file from the current script directory
--FILE--
<?php

function fn1598421520()
{
    include "inc.inc";
}
fn1598421520();
--EXPECT--
Included!
