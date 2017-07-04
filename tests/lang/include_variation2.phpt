--TEST--
Including a file in the current script directory from an included function
--FILE--
<?php

function fn768158420()
{
    require_once 'include_files/function.inc';
    test();
}
fn768158420();
--EXPECT--
Included!
