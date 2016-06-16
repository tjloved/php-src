--TEST--
Error messages are shown
--FILE--
<?php

function fn1364585110()
{
    // If this test fails ask the developers of run-test.php
    $error = 1 / 0;
}
fn1364585110();
--EXPECTREGEX--
.*Division by zero.*
