--TEST--
Bug #74840: Opcache overwrites argument of GENERATOR_RETURN within finally
--FILE--
<?php

function fn1223484335()
{
    $g = (function ($a) {
        try {
            return $a + 1;
        } finally {
            $b = $a + 2;
            var_dump($b);
        }
        yield;
        // Generator
    })(1);
    $g->next();
    var_dump($g->getReturn());
}
fn1223484335();
--EXPECT--
int(3)
int(2)
