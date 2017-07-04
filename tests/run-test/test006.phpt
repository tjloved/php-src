--TEST--
Error messages are shown
--FILE--
<?php

function fn1098530726()
{
    // If this test fails ask the developers of run-test.php
    $error = 1 / 0;
}
fn1098530726();
--EXPECTREGEX--
.*Division by zero.*
