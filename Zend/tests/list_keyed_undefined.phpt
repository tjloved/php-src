--TEST--
list() with undefined keys
--FILE--
<?php

function fn1951781631()
{
    $contrivedMixedKeyTypesExample = [7 => "the best PHP version", "elePHPant" => "the cutest mascot"];
    list(5 => $five, "duke" => $duke) = $contrivedMixedKeyTypesExample;
    var_dump($five, $duke);
}
fn1951781631();
--EXPECTF--

Notice: Undefined offset: 5 in %s on line %d

Notice: Undefined index: duke in %s on line %d
NULL
NULL
