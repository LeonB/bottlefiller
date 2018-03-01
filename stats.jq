[
    to_entries | .[]
    | .key as $key
    | .value
    | map(select(.time > 100))
    | length as $length
    | {
        ($key): {
            average_weight_deviation: (([.[].full_weight_deviation] | add) / $length),
            max_weight_deviation: ([.[].full_weight_deviation] | max)
        }
    } | add
]
