param
(
	$startX = 0,
	$y,
	$distance,
	$type,
	$count 
)

$objects = @()
for ($i = 0; $i -lt $count; $i++)
{
	$obj = @{
		name = $type;
        components = @( 
			@{
				type = "physical";
				properties = @{
					x = $i * $distance + $startX;
					y  = $y;
				}
			}
        )
	}
    $objects += $obj
}

ConvertTo-Json $objects -Depth 100 -Compress
