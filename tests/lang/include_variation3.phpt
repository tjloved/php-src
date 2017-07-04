--TEST--
Including a file in the current script directory from eval'd code
--FILE--
<?php

function fn1740705590()
{
    require_once 'include_files/eval.inc';
}
fn1740705590();
--EXPECT--
Included!
