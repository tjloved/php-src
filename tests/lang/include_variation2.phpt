--TEST--
Including a file in the current script directory from an included function
--FILE--
<?php

function fn568954714()
{
    require_once 'include_files/function.inc';
    test();
}
fn568954714();
--EXPECT--
Included!
