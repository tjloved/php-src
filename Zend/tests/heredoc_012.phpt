--TEST--
Heredoc with double quotes
--FILE--
<?php

function fn1166552195()
{
    $test = "foo";
    $var = <<<MYLABEL
test: {$test}
MYLABEL;
    echo $var;
}
fn1166552195();
--EXPECT--
test: foo
