--TEST--
Including a file in the current script directory from eval'd code
--FILE--
<?php

function fn1739840673()
{
    require_once 'include_files/eval.inc';
}
fn1739840673();
--EXPECT--
Included!
