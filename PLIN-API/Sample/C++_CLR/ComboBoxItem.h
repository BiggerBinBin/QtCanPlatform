#pragma once

namespace PLINAPIExample
{
	/// <summary>
	/// Represents a comboBoxItem with a value and its corresponding displayed value 
	/// </summary>
	public ref class ComboBoxItem
	{
	public:
        /// <summary>
        /// Displayed Value 
        /// </summary>
		property String^ Text;
        /// <summary>
        /// Value object
        /// </summary>
		property Object^ Value;

		/// <summary>
		/// Constructs and initializes a comboBoxItem
		/// </summary>
		/// <param name="text">displayed value</param>
		/// <param name="value">the value object</param>
		ComboBoxItem(String^ text, Object^ value)
		{
			this->Text = text;
			this->Value = value;
		}

		virtual String^ ToString() override
		{
			return Text;
		}
	};

}