--TEST--
include() a file from the current script directory
--FILE--
<?php

function fn55057667()
{
    include "inc.inc";
}
fn55057667();
--EXPECT--
Included!
