--TEST--
Heredoc with double quotes
--FILE--
<?php

function fn1433882141()
{
    $test = "foo";
    $var = <<<MYLABEL
test: {$test}
MYLABEL;
    echo $var;
}
fn1433882141();
--EXPECT--
test: foo
