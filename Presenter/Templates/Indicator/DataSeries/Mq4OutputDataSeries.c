	class Mq4OutputDataSeries : IMq4Array<Mq4Double>
    {
        public IndicatorDataSeries OutputDataSeries { get; private set; }
        private readonly IndicatorDataSeries _originalValues;
        private int _currentIndex;
        private int _shift;
        private double _emptyValue = double.NaN;
        private readonly DataSeriesExtremums _closeExtremums;
        private readonly ChartObjects _chartObjects;
		private readonly int _style;
		private readonly int _bufferIndex;
		private readonly ConvertedIndicator _indicator;
		private readonly Colors? _color;

        public Mq4OutputDataSeries(
			ConvertedIndicator indicator, 
			IndicatorDataSeries outputDataSeries, 
			DataSeriesExtremums closeExtremums, 
			ChartObjects chartObjects, 
			int style, 
			int bufferIndex,
			Func<IndicatorDataSeries> dataSeriesFactory,
			Colors? color = null)
        {
            OutputDataSeries = outputDataSeries;
            _closeExtremums = closeExtremums;
            _chartObjects = chartObjects;
			_style = style;
			_bufferIndex = bufferIndex;
			_indicator = indicator;
			_color = color;
			_originalValues = dataSeriesFactory();
        }

        public int Length
        {
            get 
            {
                return OutputDataSeries.Count;
            }
        }

        public void SetCurrentIndex(int index)
        {
            _currentIndex = index;
        }        
        
        public void SetShift(int shift)
        {
          _shift = shift;
        }
        
        public void SetEmptyValue(double emptyValue)
        {
          _emptyValue = emptyValue;
        }

        public Mq4Double this[int index]
        {
            get 
            { 
                if (index >= _originalValues.Count)
                  return 0;
                        
                return _originalValues[_currentIndex - index + _shift];
            }
            set 
            { 
                var indexToSet = _currentIndex - index + _shift;                
                _originalValues[indexToSet] = value;

                var valueToSet = value;
                if (valueToSet == _emptyValue)
                  valueToSet = double.NaN;

                if (indexToSet < 0)
                  return;

                if (#IsDrawingOnChartWindow_PLACE_HOLDER#)
                {
                    var validRange = _closeExtremums.Max - _closeExtremums.Min;                
                    if (value > _closeExtremums.Max + validRange || value < _closeExtremums.Min - validRange)
                        return;
                }
					
				switch (_style)
				{
					case DRAW_ARROW:
						var arrowName = GetArrowName(indexToSet);
						if (double.IsNaN(valueToSet))
							_chartObjects.RemoveObject(arrowName);
						else
						{
							var color = _color.HasValue ? _color.Value : Colors.Red;
							_chartObjects.DrawText(arrowName , _indicator.ArrowByIndex[_bufferIndex], indexToSet, valueToSet, VerticalAlignment.Center, HorizontalAlignment.Center, color);
						}
						break;
				}

                OutputDataSeries[indexToSet] = valueToSet; 
            }
        }
		
        private string GetOverlapLineName(int startIndex)
        {
            return string.Format("Overlapline {0} {1}", GetHashCode(), startIndex);
        }
		
        private string GetArrowName(int index)
        {
            return string.Format("Arrow {0} {1}", GetHashCode(), index);
        }
    }